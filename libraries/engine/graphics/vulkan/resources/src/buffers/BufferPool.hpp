#pragma once

#include <deque>
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "resources/BufferRegion.hpp"
#include "resources/PerFrameBufferHandles.hpp"
#include "resources/ResizeRequest.hpp"
#include "bk/NonCopyMove.hpp"
#include "core/AllocatorService.hpp"
#include "engine/common/HandleGenerator.hpp"
#include "common/BufferCreateInfo.hpp"
#include "resources/BufferHandle.hpp"

namespace arb {
class AllocatorService;
struct Buffer;
class TransferSystem;
class BufferAllocator;
class FrameManager;
class Device;

struct BufferEntry {
  BufferLifetime lifetime;
  std::deque<std::unique_ptr<Buffer>> versions;
  size_t currentSize{};
};

class BufferPool : public NonCopyableMovable {
public:
  BufferPool(Device& newDevice, AllocatorService& newAllocatorService);
  ~BufferPool();

  auto registerBuffer(const BufferCreateInfo& createInfo) -> BufferHandle;

  auto registerPerFrameBuffer(const BufferCreateInfo& createInfo, uint32_t frameCount)
      -> PerFrameBufferHandles;

  auto insert(BufferHandle handle, void* data, const BufferRegion& targetRegion)
      -> std::optional<BufferRegion>;

  auto checkSize(BufferHandle handle, size_t size) -> std::optional<ResizeRequest>;

  auto markCurrentBufferInFlight(BufferHandle handle,
                                 VkSemaphore queueSemaphore,
                                 uint64_t signaledValue) -> void;

  auto processResizes(const std::vector<ResizeRequest>& resizeRequests,
                      TransferSystem& transferSystem) -> void;

  auto tick() -> void;

  [[nodiscard]] auto resolve(LogicalBufferHandle logicalHandle) const -> BufferHandle;

  [[nodiscard]] auto getVkBuffer(BufferHandle handle) const -> VkBuffer;

  [[nodiscard]] auto getBuffer(BufferHandle handle) const -> Buffer&;

private:
  Device& device;
  AllocatorService& allocatorService;
  HandleGenerator<BufferTag> bufferHandleGenerator;
  std::unique_ptr<BufferAllocator> bufferAllocator;

  std::unordered_map<BufferHandle, std::unique_ptr<BufferEntry>> bufferMap;

  static auto fromCreateInfo(const BufferCreateInfo& info)
      -> std::tuple<VkBufferCreateInfo, VmaAllocationCreateInfo>;

  auto getCurrentBuffer(Handle<BufferTag> handle, VkSemaphore usageQueueSemaphore)
      -> std::optional<Buffer*>;

  auto isBufferMappable(const Buffer* buffer) -> bool;
};
}
