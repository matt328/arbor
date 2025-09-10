#pragma once

#include "BufferHandle.hpp"
#include "bk/NonCopyMove.hpp"
#include "BufferCreateInfo.hpp"

#include <memory>

namespace arb {

class BufferManager;

class ResourceContext : public NonCopyableMovable {
public:
  ResourceContext();
  ~ResourceContext();

  auto createBuffer(const BufferCreateInfo& info) -> BufferHandle;
  auto createPerFrameBuffer(const BufferCreateInfo& info) -> LogicalBufferHandle;
  auto getBuffer(LogicalBufferHandle logicalHandle, size_t frameIndex) -> BufferHandle;

private:
  std::unique_ptr<BufferManager> bufferManager;
};

}
