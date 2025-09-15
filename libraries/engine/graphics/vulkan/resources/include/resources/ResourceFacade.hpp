#pragma once

#include <optional>

#include "Buffer.hpp"
#include "BufferHandle.hpp"
#include "bk/NonCopyMove.hpp"
#include "BufferCreateInfo.hpp"
#include "resources/BufferRegion.hpp"
#include "resources/ImageHandle.hpp"
#include "resources/PerFrameBufferHandles.hpp"
#include "Image.hpp"
#include "resources/ResizeRequest.hpp"

namespace arb {

class BufferManager;

class ResourceFacade : public NonCopyableMovable {
public:
  ResourceFacade(BufferManager& newBufferManager);
  ~ResourceFacade();

  auto createBuffer(const BufferCreateInfo& info) -> BufferHandle;
  auto createPerFrameBuffer(const BufferCreateInfo& info, uint32_t frameCount)
      -> PerFrameBufferHandles;
  auto getBufferHandle(LogicalBufferHandle logicalHandle, size_t frameIndex) -> BufferHandle;
  auto getBuffer(LogicalBufferHandle logicalHandle, size_t frameIndex) -> Buffer&;
  auto getBuffer(BufferHandle handle) -> Buffer&;
  auto checkBufferSize(BufferHandle handle, size_t size) -> std::optional<ResizeRequest>;
  auto allocateInBuffer(BufferHandle handle, size_t size) -> BufferRegion;

  auto getImageHandle(LogicalImageHandle logicalHandle, size_t frameIndex) -> ImageHandle;
  auto getImage(LogicalImageHandle logicalHandle, size_t frameIndex) -> Image&;
  auto getImage(ImageHandle handle) -> Image&;

private:
  BufferManager& bufferManager;
};

}
