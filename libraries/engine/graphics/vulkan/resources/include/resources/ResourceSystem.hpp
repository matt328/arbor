#pragma once

#include <optional>

#include "Buffer.hpp"
#include "BufferHandle.hpp"
#include "bk/NonCopyMove.hpp"
#include "BufferCreateInfo.hpp"
#include "core/ImageView.hpp"
#include "engine/common/RenderSurfaceState.hpp"
#include "resources/BufferRegion.hpp"
#include "core/ImageHandle.hpp"
#include "resources/PerFrameBufferHandles.hpp"
#include "core/Image.hpp"
#include "resources/ResizeRequest.hpp"
#include "resources/images/ImageSystem.hpp"

namespace arb {

class BufferManager;

class ResourceSystem : public NonCopyableMovable {
public:
  explicit ResourceSystem(BufferManager& newBufferManager, ImageSystem& newImageSystem);
  ~ResourceSystem();

  void resize(const RenderSurfaceState& surfaceState);

  // Buffers
  auto createBuffer(const BufferCreateInfo& info) -> BufferHandle;
  auto createPerFrameBuffer(const BufferCreateInfo& info, uint32_t frameCount)
      -> PerFrameBufferHandles;
  auto getBufferHandle(LogicalBufferHandle logicalHandle, size_t frameIndex) -> BufferHandle;
  auto getBuffer(LogicalBufferHandle logicalHandle, size_t frameIndex) -> Buffer&;
  auto getBuffer(BufferHandle handle) -> Buffer&;
  auto checkBufferSize(BufferHandle handle, size_t size) -> std::optional<ResizeRequest>;
  auto allocateInBuffer(BufferHandle handle, size_t size) -> BufferRegion;

  // Images
  auto createImage(const ImageCreateDescription& imageSpec) -> ImageHandle;
  auto createImageView(const ImageViewSpec& imageViewSpec) -> ImageViewHandle;

  auto resolveImageHandle(LogicalImageHandle logicalHandle, uint32_t frameIndex) -> ImageHandle;
  auto getImage(LogicalImageHandle logicalHandle, size_t frameIndex) -> const Image&;
  auto getImage(ImageHandle handle) -> const Image&;
  auto getImageView(LogicalImageViewHandle logicalHandle, uint32_t frameIndex) -> const ImageView&;
  auto getImageView(ImageViewHandle handle) -> const ImageView&;

private:
  BufferManager& bufferManager;
  ImageSystem& imageSystem;
};

}
