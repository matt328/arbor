#include "BufferPool.hpp"

#include <cassert>
#include <deque>
#include <ranges>

#include "resources/Buffer.hpp"
#include "bk/Logger.hpp"
#include "core/Device.hpp"
#include "core/AllocatorService.hpp"
#include "BufferAllocator.hpp"

namespace arb {

BufferPool::BufferPool(Device& newDevice, AllocatorService& newAllocatorService)
    : device{newDevice}, allocatorService{newAllocatorService} {
  Log::trace("Creating BufferPool");
  bufferAllocator = std::make_unique<BufferAllocator>();
}

BufferPool::~BufferPool() {
  Log::trace("Destroying BufferPool");
}

auto BufferPool::registerBuffer(const BufferCreateInfo& createInfo) -> BufferHandle {
  const auto handle = bufferHandleGenerator.requestHandle();
  const auto name = std::format("{}-v0", createInfo.debugName);

  auto [bci, aci] = fromCreateInfo(createInfo);

  auto versions = std::deque<std::unique_ptr<Buffer>>{};
  versions.push_back(Buffer::create(allocatorService, bci, aci, name));

  bufferMap.emplace(
      handle,
      std::make_unique<BufferEntry>(BufferEntry{.lifetime = createInfo.bufferLifetime,
                                                .versions = std::move(versions),
                                                .currentSize = createInfo.initialSize}));

  bufferAllocator->registerBuffer(handle,
                                  createInfo.allocationStrategy,
                                  createInfo.initialSize,
                                  name);
  return handle;
}

auto BufferPool::registerPerFrameBuffer(const BufferCreateInfo& createInfo, uint32_t frameCount)
    -> PerFrameBufferHandles {
  const auto logicalHandle = bufferHandleGenerator.requestLogicalHandle();

  auto handles = PerFrameBufferHandles{.logicalHandle = logicalHandle};
  handles.bufferHandles.reserve(frameCount);

  for (uint32_t i = 0; i < frameCount; ++i) {
    handles.bufferHandles.push_back(registerBuffer(createInfo));
  }

  return handles;
}

auto BufferPool::insert(BufferHandle handle, void* data, const BufferRegion& targetRegion)
    -> std::optional<BufferRegion> {
  std::optional<BufferRegion> region = std::nullopt;
  if (bufferMap.at(handle)->lifetime != BufferLifetime::Transient) {
    return region;
  }

  auto buffer = getCurrentBuffer(handle, VK_NULL_HANDLE);

  auto* buf = *buffer;
  assert(isBufferMappable(buf));
  if (buf->mappedData == nullptr) {
    auto* mappedMemory = allocatorService.mapMemory(buf->allocation);
    assert(data != nullptr);
    if (targetRegion.offset + targetRegion.size > buf->bufferMeta.bufferCreateInfo.size) {
      Log::warn("Buffer {} Full", buf->bufferMeta.debugName);
    }
    assert(targetRegion.offset + targetRegion.size > buf->bufferMeta.bufferCreateInfo.size);
    char* dest = static_cast<char*>(mappedMemory);
    if (dest != nullptr) {
      std::memcpy(dest + targetRegion.offset, data, targetRegion.size);
      region.emplace(targetRegion);
    }
  }

  return region;
}

auto checkSize(BufferHandle handle, size_t size) -> std::optional<ResizeRequest> {
  return std::nullopt;
}

auto BufferPool::markCurrentBufferInFlight(BufferHandle handle,
                                           VkSemaphore queueSemaphore,
                                           uint64_t signaledValue) -> void {
  auto buffer = getCurrentBuffer(handle, queueSemaphore);
  if (!buffer) {
    return;
  }
  auto* buf = *buffer;
  buf->pendingValues[queueSemaphore] = signaledValue;
}

auto processResizes(const std::vector<ResizeRequest>& resizeRequests,
                    TransferSystem& transferSystem) -> void;

auto BufferPool::getCurrentBuffer(Handle<BufferTag> handle, VkSemaphore usageQueueSemaphore)
    -> std::optional<Buffer*> {
  assert(bufferMap.contains(handle));
  const auto& entry = bufferMap.at(handle);
  std::optional<Buffer*> buffer = std::nullopt;

  if (entry->lifetime == BufferLifetime::Transient) {
    buffer.emplace(entry->versions.front().get());
  } else {
    for (const auto& buf : entry->versions | std::ranges::views::reverse) {
      bool ready = true;
      for (auto& [sem, val] : buf->pendingValues) {
        uint64_t current{};
        vkGetSemaphoreCounterValue(device, sem, &current);
        if (sem == usageQueueSemaphore && current < val) {
          ready = false;
          break;
        }
      }
      if (ready) {
        buffer.emplace(buf.get());
      }
    }
  }
  return buffer;
}

auto BufferPool::tick() -> void {
  for (auto& [handle, entry] : bufferMap) {
    for (auto& version : entry->versions) {
      for (auto it = version->pendingValues.begin(); it != version->pendingValues.end();) {
        uint64_t value{};
        auto current = vkGetSemaphoreCounterValue(device, it->first, &value);
        if (current >= it->second) {
          it = version->pendingValues.erase(it);
        } else {
          ++it;
        }
      }
      while (!entry->versions.empty() && entry->versions.front()->pendingValues.empty()) {
        entry->versions.pop_front();
      }
    }
  }
}

auto BufferPool::fromCreateInfo(const BufferCreateInfo& info)
    -> std::tuple<VkBufferCreateInfo, VmaAllocationCreateInfo> {
  auto bci = VkBufferCreateInfo{.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                                .size = info.initialSize,
                                .usage = VK_BUFFER_USAGE_2_SHADER_DEVICE_ADDRESS_BIT};
  auto aci = VmaAllocationCreateInfo{};

  switch (info.bufferUsage) {
    case BufferUsage::Storage:
      bci.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
      break;
    case BufferUsage::Transfer:
      bci.usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
      break;
    case BufferUsage::Uniform:
      bci.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
      break;
  }

  if (info.bufferLifetime == BufferLifetime::Persistent) {
    bci.usage |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    aci.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    aci.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
  } else if (info.bufferLifetime == BufferLifetime::Transient) {
    aci.usage = VMA_MEMORY_USAGE_CPU_ONLY;
    aci.requiredFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
  }

  if (info.indirect) {
    bci.usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
  }

  if (info.allocationStrategy == AllocationStrategy::Arena ||
      info.allocationStrategy == AllocationStrategy::Resizable) {
    bci.usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  }

  return {bci, aci};
}

auto BufferPool::isBufferMappable(const Buffer* buffer) -> bool {
  const auto memProps = allocatorService.getAllocationMemoryProperties(buffer->allocation);
  return (memProps & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0u;
}

}
