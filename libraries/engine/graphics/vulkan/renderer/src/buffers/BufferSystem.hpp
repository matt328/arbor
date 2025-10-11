#pragma once

#include <deque>
#include <memory>
#include <optional>

#include "buffers/LogicalBufferMapper.hpp"
#include "bk/NonCopyMove.hpp"
#include "common/BufferCreateInfo.hpp"
#include "core/AllocatorService.hpp"
#include "engine/common/HandleGenerator.hpp"
#include "buffers/BufferHandle.hpp"
#include "buffers/BufferRegion.hpp"
#include "buffers/ResizeRequest.hpp"
#include "core/Buffer.hpp"
#include "IBufferAllocator.hpp"

namespace arb {

class Device;
class TransferSystem;

struct ResizeJob {
  BufferHandle handle;
  Buffer& oldBuffer;
  std::unique_ptr<Buffer> newBuffer;
  size_t newSize;
};

struct BufferEntry {
  BufferLifetime lifetime;
  std::deque<std::unique_ptr<Buffer>> versions;
  size_t currentSize{0};
};

struct BufferSystemDeps {
  Device& device;
  AllocatorService& allocatorService;
  TransferSystem& transferSystem;
};

class BufferSystem : public NonCopyableMovable {
public:
  explicit BufferSystem(const BufferSystemDeps& deps);
  ~BufferSystem();

  /// Registers a single buffer based on the BufferRequest, and returns a Handle to the buffer which
  /// can be used in subsequent method calls to work with the created buffer.
  auto registerBuffer(const BufferCreateInfo& createInfo) -> BufferHandle;

  /// Registers (framesCount) buffers with each frame by consulting the FrameManager. Takes in A
  /// `BufferCreateInfo` that describes the buffer and returns a `LogicalBufferHandle`, which must
  /// be exchanged for a BufferHandle with a Frame.
  auto registerPerFrameBuffer(const BufferCreateInfo& createInfo, uint32_t frameCount)
      -> LogicalBufferHandle;

  auto getBuffer(BufferHandle handle) -> Buffer&;

  /// Write `data` into a PerFrame buffer, specified by `handle` at the given `BufferRegion`.
  /// This is intended for PerFrame host visible buffers only. Happens synchronously and does not
  /// use a staging buffer or transfer queue.
  auto insert(BufferHandle handle, void* data, const BufferRegion& targetRegion)
      -> std::optional<BufferRegion>;

  /// Removes the data from the buffer described by the given `BufferRegion`. Should be quick as it
  /// doesn't change the buffer on the GPU, but just returns the given region to the free list on
  /// the CPU side. Not fully implemented yet.
  void removeData(BufferHandle handle, const BufferRegion& region);

  /// Checks whether the given buffer requires a resize before `size` data can be inserted. Defers
  /// to buffer's individual resize policies for the exact logic.
  auto checkSize(BufferHandle handle, size_t size) -> std::optional<ResizeRequest>;

  /// Resizes the given buffer by allocating a new buffer of `newSize` and copying the existing
  /// buffer contents into it. Executes synchronously on the current thread. BufferSystem will
  /// handle replacing the buffer transparently to clients, always presenting a consistent buffer
  /// and ensuring the frames in flight are flushed before deleting the old buffer.
  void processResizes(const std::vector<ResizeRequest>& resizeRequests);

  /// Attempts to reserve a `BufferRegion` of `size` in the buffer given by `handle`. On success,
  /// returns a `BufferRegion`. This method will assert if there is not enough room, and clients
  /// need to explicitly use `checkSize` and batch resize requests when possible.
  auto allocate(BufferHandle handle, size_t size) -> BufferRegion;

  /// Call once per frame to recycle or remove buffers that have been resized
  void update();

private:
  Device& device;
  AllocatorService& allocatorService;
  TransferSystem& transferSystem;

  HandleGenerator<BufferTag> bufferHandleGenerator;

  std::unique_ptr<LogicalBufferMapper> bufferMapper;

  std::unordered_map<BufferHandle, std::unique_ptr<BufferEntry>> bufferMap;
  std::unordered_map<BufferHandle, std::unique_ptr<IBufferAllocator>> allocatorMap;

  [[nodiscard]] auto getCurrentBuffer(BufferHandle handle) -> Buffer&;

  [[nodiscard]] auto isBufferMappable(const Buffer& buffer) const -> bool;

  void prepareResizeJobs(const std::vector<ResizeRequest>& resizeRequests,
                         std::vector<ResizeJob>& jobs,
                         std::vector<std::tuple<Buffer&, Buffer&>>& copyPairs);

  auto tryGetBufferEntry(BufferHandle handle) -> Buffer*;
  auto createResizedBuffer(const Buffer& oldBuffer, size_t newSize) -> std::unique_ptr<Buffer>;
  void finalizeResizeJobs(std::vector<ResizeJob>& jobs, uint64_t completed);

  static auto fromCreateInfo(const BufferCreateInfo& createInfo)
      -> std::tuple<VkBufferCreateInfo, VmaAllocationCreateInfo>;
};

}
