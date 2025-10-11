#pragma once

#include <optional>

#include "bk/NonCopyMove.hpp"
#include "buffers/BufferHandle.hpp"
#include "buffers/ResizeRequest.hpp"
#include "buffers/BufferRegion.hpp"
#include "buffers/BufferRequest.hpp"

namespace arb {

class IBufferAllocator : public NonCopyableMovable {
public:
  explicit IBufferAllocator(BufferHandle handle) : bufferHandle{handle} {
  }
  virtual ~IBufferAllocator() = default;

  virtual auto allocate(const BufferRequest& requestData) -> BufferRegion = 0;
  virtual auto checkSize(const BufferRequest& requestData) -> std::optional<ResizeRequest> = 0;
  virtual auto notifyBufferResized(size_t newSize) -> void = 0;
  virtual auto freeRegion(const BufferRegion& region) -> void = 0;
  virtual auto reset() -> void = 0;

protected:
  BufferHandle bufferHandle;
};
}
