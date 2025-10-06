#include "BufferManager.hpp"

#include "bk/Log.hpp"
#include "buffers/BufferPool.hpp"
#include "core/AllocatorService.hpp"
#include "core/Device.hpp"

namespace arb {

BufferManager::BufferManager(Device& newDevice,
                             AllocatorService& newAllocatorService,
                             SemaphorePack semaphores) {
  LOG_TRACE_L1(Log::Resources, "Creating BufferManager");
  bufferPool = std::make_unique<BufferPool>(newDevice, newAllocatorService);
}

BufferManager::~BufferManager() {
  LOG_TRACE_L1(Log::Resources, "Destroying BufferManager");
}

auto BufferManager::registerBuffer(const BufferCreateInfo& createInfo) -> BufferHandle {
  return bufferPool->registerBuffer(createInfo);
}

auto BufferManager::registerPerFrameBuffer(const BufferCreateInfo& createInfo, uint32_t frameCount)
    -> PerFrameBufferHandles {
  return bufferPool->registerPerFrameBuffer(createInfo, frameCount);
}

auto BufferManager::insert(BufferHandle handle, void* data, const BufferRegion& targetRegion)
    -> std::optional<BufferRegion> {
  return bufferPool->insert(handle, data, targetRegion);
}

auto BufferManager::allocate(BufferHandle handle, size_t size) -> BufferRegion {
  return {};
}

auto BufferManager::checkSize(BufferHandle handle, size_t size) -> std::optional<ResizeRequest> {
  return std::nullopt;
  // return virtualAllocationManager->checkSizes(handle, size);
}

auto BufferManager::processResizes(const std::vector<ResizeRequest>& resizeRequests,
                                   TransferSystem& transferSystem) -> void {
}

auto BufferManager::resolve(LogicalBufferHandle logicalHandle, size_t frameIndex) const
    -> BufferHandle {
}

auto BufferManager::getVkBuffer(BufferHandle handle) const -> VkBuffer {
  return bufferPool->getVkBuffer(handle);
}

auto BufferManager::getBuffer(BufferHandle handle) const -> Buffer& {
  return bufferPool->getBuffer(handle);
}

}
