#include "BufferSystem.hpp"

#include <cassert>
#include <cpptrace/cpptrace.hpp>
#include <ranges>
#include <vulkan/vulkan_core.h>

#include "LinearAllocator.hpp"
#include "ArenaAllocator.hpp"
#include "LogicalBufferMapper.hpp"

#include "bk/Log.hpp"
#include "bk/Strings.hpp"

#include "common/BufferCreateInfo.hpp"
#include "core/Device.hpp"
#include "core/AllocatorService.hpp"
#include "transfer/TransferSystem.hpp"

namespace arb {

BufferSystem::BufferSystem(const BufferSystemDeps& deps)
    : device{deps.device},
      allocatorService{deps.allocatorService},
      transferSystem{deps.transferSystem} {
  LOG_TRACE_L1(Log::Renderer, "Creating BufferSystem");
  bufferMapper = std::make_unique<LogicalBufferMapper>();
}

BufferSystem::~BufferSystem() {
  LOG_TRACE_L1(Log::Renderer, "Destroying BufferSystem");
  device.waitIdle();
  allocatorMap.clear();
  bufferMap.clear();
}

auto BufferSystem::registerBuffer(const BufferCreateInfo& createInfo) -> BufferHandle {
  const auto handle = bufferHandleGenerator.requestHandle();
  const auto name = std::format("{}-v0", createInfo.debugName);
  const auto size = createInfo.initialSize;
  const auto [bci, aci] = fromCreateInfo(createInfo);

  auto versions = std::deque<std::unique_ptr<Buffer>>{};
  versions.push_back(std::make_unique<Buffer>(allocatorService, bci, aci, name));

  bufferMap.emplace(handle,
                    std::make_unique<BufferEntry>(BufferEntry{.lifetime = createInfo.bufferLifetime,
                                                              .versions = std::move(versions),
                                                              .currentSize = size}));
  switch (createInfo.allocationStrategy) {
    case AllocationStrategy::Linear:
      allocatorMap.emplace(handle, std::make_unique<LinearAllocator>(handle, size, name));
      break;
    case AllocationStrategy::Arena:
      allocatorMap.emplace(handle, std::make_unique<ArenaAllocator>(handle, size, name));
      break;
    case AllocationStrategy::Resizable:
      break;
  }
  return handle;
}

auto BufferSystem::registerPerFrameBuffer(const BufferCreateInfo& createInfo, uint32_t frameCount)
    -> LogicalBufferHandle {
  const auto logicalHandle = bufferHandleGenerator.requestLogicalHandle();
  for (uint32_t i = 0; i < frameCount; ++i) {
    const auto handle = registerBuffer(createInfo);
    bufferMapper->registerPerFrameBuffer(logicalHandle, handle, i);
  }
  return logicalHandle;
}

auto BufferSystem::getBuffer(BufferHandle handle) -> Buffer& {
  return getCurrentBuffer(handle);
}

auto BufferSystem::getPerFrameBuffer(LogicalBufferHandle handle, uint32_t frameIndex) -> Buffer& {
  auto physicalHandle = bufferMapper->resolveBufferHandle(handle, frameIndex);
  return getCurrentBuffer(physicalHandle);
}

auto BufferSystem::resolveHandle(LogicalBufferHandle handle, uint32_t frameIndex) -> BufferHandle {
  return bufferMapper->resolveBufferHandle(handle, frameIndex);
}

auto BufferSystem::insert(BufferHandle handle, void* data, const BufferRegion& targetRegion)
    -> std::optional<BufferRegion> {
  std::optional<BufferRegion> region = std::nullopt;
  if (bufferMap.at(handle)->lifetime != BufferLifetime::Transient) {
    return region;
  }

  auto& buffer = getCurrentBuffer(handle);

  assert(buffer.isMappable());
  if (!buffer.isMapped()) {
    buffer.map();
    assert(data != nullptr);
    if (targetRegion.offset + targetRegion.size > buffer.getSize()) {
      LOG_WARNING(Log::Core, "Buffer {} Full", buffer.getDebugName());
    }
    assert(targetRegion.offset + targetRegion.size > buffer.getSize());

    auto dest2 = std::span{static_cast<std::byte*>(buffer.getMappedData()), buffer.getSize()};
    if (targetRegion.offset + targetRegion.size > dest2.size_bytes()) {
      LOG_WARNING(Log::Core, "Buffer {} Full", buffer.getDebugName());
      assert(false);
    }
    std::memcpy(dest2.subspan(targetRegion.offset, targetRegion.size).data(),
                data,
                targetRegion.size);
    region.emplace(targetRegion);
  }

  return region;
}

void BufferSystem::removeData(BufferHandle handle, const BufferRegion& region) {
  assert(bufferMap.contains(handle));
  if (allocatorMap.contains(handle)) {
    allocatorMap.at(handle)->freeRegion(region);
  } else {
    LOG_WARNING(Log::Renderer, "removeData called with a buffer that has no allocator registered");
  }
}

auto BufferSystem::checkSize(BufferHandle handle, size_t size) -> std::optional<ResizeRequest> {
  if (!allocatorMap.contains(handle)) {
    LOG_WARNING(Log::Resources,
                "Check size of a buffer that doesn't have an allocator, handle={}",
                handle.id);
  }
  return allocatorMap.at(handle)->checkSize({.size = size});
}

void BufferSystem::processResizes(const std::vector<ResizeRequest>& resizeRequests) {
  std::vector<ResizeJob> jobs;
  std::vector<std::tuple<Buffer&, Buffer&>> copyPairs;

  prepareResizeJobs(resizeRequests, jobs, copyPairs);

  if (jobs.empty()) {
    return;
  }

  const auto completed = transferSystem.submitCopyBatch(copyPairs);
  finalizeResizeJobs(jobs, completed);
}

auto BufferSystem::allocate(BufferHandle handle, size_t size) -> BufferRegion {
  assert(bufferMap.contains(handle));

  const auto lifetime = bufferMap.at(handle)->lifetime;
  if (lifetime == BufferLifetime::Persistent) {
    assert(allocatorMap.contains(handle));
    auto& bufferAllocator = allocatorMap.at(handle);
    return bufferAllocator->allocate(BufferRequest{.size = size});
  }
  assert("Tried to allocate into a transient buffer");
  return BufferRegion{};
}

void BufferSystem::update() {
  const auto completedValue = transferSystem.getCurrentCopyValue();

  for (auto& [handle, entry] : bufferMap) {
    if (entry->lifetime == BufferLifetime::Transient) {
      continue;
    }
    auto& versions = entry->versions;
    std::erase_if(versions,
                  [&](auto& buffer) { return buffer->getValidToValue() <= completedValue; });
  }
}

auto BufferSystem::getCurrentBuffer(BufferHandle handle) -> Buffer& {
  assert(bufferMap.contains(handle));
  auto& entry = bufferMap.at(handle);

  if (entry->lifetime == BufferLifetime::Transient) {
    assert(!entry->versions.empty());
    return *entry->versions.front();
  }

  const auto currentValue = transferSystem.getCurrentCopyValue();
  for (const auto& rec : entry->versions | std::ranges::views::reverse) {
    if (rec->getValidFromValue() <= currentValue && rec->getValidToValue() > currentValue) {
      return *rec;
    }
  }

  // If you reach here, it's a logic error — no buffer is valid for this handle.
  throw cpptrace::runtime_error(std::format("No valid buffer found for handle {}", handle.id));
}

void BufferSystem::prepareResizeJobs(const std::vector<ResizeRequest>& resizeRequests,
                                     std::vector<ResizeJob>& jobs,
                                     std::vector<std::tuple<Buffer&, Buffer&>>& copyPairs) {
  for (const auto& resize : resizeRequests) {
    auto oldBuffer = getCurrentBuffer(resize.bufferHandle);
    LOG_TRACE_L1(Log::Renderer,
                 "Resizing Buffer: {}, newSize={}",
                 oldBuffer.getDebugName(),
                 resize.newSize);
    auto newBuffer = createResizedBuffer(oldBuffer, resize.newSize);
    copyPairs.emplace_back(oldBuffer, *newBuffer);
    jobs.push_back(ResizeJob{.handle = resize.bufferHandle,
                             .oldBuffer = oldBuffer,
                             .newBuffer = std::move(newBuffer),
                             .newSize = resize.newSize});
  }
}

auto BufferSystem::createResizedBuffer(const Buffer& oldBuffer, size_t newSize)
    -> std::unique_ptr<Buffer> {
  auto bci = oldBuffer.getCreateInfo();
  bci.size = newSize;
  auto aci = oldBuffer.getAllocationCreateInfo();
  auto newName = bk::strings::incrementVersion(oldBuffer.getDebugName());
  return std::make_unique<Buffer>(allocatorService, bci, aci, newName);
}

void BufferSystem::finalizeResizeJobs(std::vector<ResizeJob>& jobs, uint64_t completed) {
  for (auto& job : jobs) {
    job.newBuffer->setValidFromValue(completed);
    job.oldBuffer.setValidToValue(completed);

    auto& entry = bufferMap.at(job.handle);
    entry->versions.emplace_back(std::move(job.newBuffer));
    entry->currentSize = job.newSize;
    entry->versions.back()->setSize(job.newSize);

    auto it = allocatorMap.find(job.handle);
    if (it != allocatorMap.end()) {
      it->second->notifyBufferResized(job.newSize);
    } else {
      LOG_WARNING(Log::Renderer, "Resized a buffer with no allocator: {}", job.handle.id);
    }
  }
}

auto BufferSystem::fromCreateInfo(const BufferCreateInfo& createInfo)
    -> std::tuple<VkBufferCreateInfo, VmaAllocationCreateInfo> {
  auto bci = VkBufferCreateInfo{.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                                .size = createInfo.initialSize,
                                .usage = VK_BUFFER_USAGE_2_SHADER_DEVICE_ADDRESS_BIT};
  auto aci = VmaAllocationCreateInfo{};

  switch (createInfo.bufferUsage) {
    case BufferUsage::Storage:
      bci.usage |= VK_BUFFER_USAGE_2_STORAGE_BUFFER_BIT;
      break;
    case BufferUsage::Transfer:
      bci.usage |= VK_BUFFER_USAGE_2_TRANSFER_SRC_BIT;
      break;
    case BufferUsage::Uniform:
      bci.usage |= VK_BUFFER_USAGE_2_UNIFORM_BUFFER_BIT;
      break;
  }

  if (createInfo.bufferLifetime == BufferLifetime::Persistent) {
    bci.usage |= VK_BUFFER_USAGE_2_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_2_TRANSFER_DST_BIT;
    aci.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    aci.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
  } else if (createInfo.bufferLifetime == BufferLifetime::Transient) {
    aci.usage = VMA_MEMORY_USAGE_CPU_ONLY;
    aci.requiredFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
  }

  if (createInfo.indirect) {
    bci.usage |= VK_BUFFER_USAGE_2_INDIRECT_BUFFER_BIT;
  }

  if (createInfo.allocationStrategy == AllocationStrategy::Arena ||
      createInfo.allocationStrategy == AllocationStrategy::Resizable) {
    bci.usage |= VK_BUFFER_USAGE_2_TRANSFER_SRC_BIT;
  }

  return {bci, aci};
}

}
