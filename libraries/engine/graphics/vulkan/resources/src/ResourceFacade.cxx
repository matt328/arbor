#include "resources/ResourceFacade.hpp"

#include "bk/Logger.hpp"

#include "buffers/BufferManager.hpp"

namespace arb {

ResourceFacade::ResourceFacade(BufferManager& newBufferManager) : bufferManager{newBufferManager} {
  Log::trace("Creating ResourceFacade");
}

ResourceFacade::~ResourceFacade() {
  Log::trace("Destroying Resource Facade");
}

auto ResourceFacade::createBuffer(const BufferCreateInfo& info) -> BufferHandle {
  return bufferManager.registerBuffer(info);
}

auto ResourceFacade::createPerFrameBuffer(const BufferCreateInfo& info, uint32_t frameCount)
    -> PerFrameBufferHandles {
  return bufferManager.registerPerFrameBuffer(info, frameCount);
}

auto ResourceFacade::getBufferHandle(LogicalBufferHandle logicalHandle, size_t frameIndex)
    -> BufferHandle {
  return bufferManager.resolve(logicalHandle, frameIndex);
}

auto ResourceFacade::getBuffer(LogicalBufferHandle logicalHandle, size_t frameIndex) -> Buffer& {
  const auto bufferHandle = bufferManager.resolve(logicalHandle, frameIndex);
  return bufferManager.getBuffer(bufferHandle);
}

}
