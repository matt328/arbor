#pragma once

#include <memory>
#include <optional>
#include <vulkan/vulkan_core.h>

#include "resources/BufferRegion.hpp"
#include "resources/ResizeRequest.hpp"
#include "bk/NonCopyMove.hpp"
#include "resources/Buffer.hpp"
#include "resources/BufferCreateInfo.hpp"
#include "resources/BufferHandle.hpp"
#include "common/SemaphorePack.hpp"
#include "resources/PerFrameBufferHandles.hpp"

namespace arb {

class AllocatorService;
class BufferPool;
class TransferSystem;
class VirtualAllocationManager;
class Device;

class BufferManager : public NonCopyableMovable {
public:
  BufferManager(Device& newDevice, AllocatorService& newAllocatorService, SemaphorePack semaphores);
  ~BufferManager();

  auto registerBuffer(const BufferCreateInfo& createInfo) -> BufferHandle;

  auto registerPerFrameBuffer(const BufferCreateInfo& createInfo, uint32_t frameCount)
      -> PerFrameBufferHandles;

  /// Attempts to reserve a BufferRegion of `size` in the buffer given by `handle`. On success, the
  /// `AllocationResult` will contain a `BufferRegion`. If the buffer must be resized first, the
  /// `AllocationResult` will contain a `ResizeRequest`. Allocation and resizing are separated out
  /// from `BufferSystem::insert` since clients may want to batch buffer resize operations.
  auto allocate(BufferHandle handle, size_t size) -> BufferRegion;

  auto insert(BufferHandle handle, void* data, const BufferRegion& targetRegion)
      -> std::optional<BufferRegion>;

  auto checkSize(BufferHandle handle, size_t size) -> std::optional<ResizeRequest>;

  auto processResizes(const std::vector<ResizeRequest>& resizeRequests,
                      TransferSystem& transferSystem) -> void;

  [[nodiscard]] auto resolve(LogicalBufferHandle logicalHandle, size_t frameIndex) const
      -> BufferHandle;

  [[nodiscard]] auto getVkBuffer(BufferHandle handle) const -> VkBuffer;
  [[nodiscard]] auto getBuffer(BufferHandle handle) const -> Buffer&;

private:
  std::unique_ptr<BufferPool> bufferPool;
};

}
