#include "resources/ResourceSystem.hpp"

#include "bk/Logger.hpp"

#include "buffers/BufferManager.hpp"

namespace arb {

ResourceSystem::ResourceSystem(BufferManager& newBufferManager, ImageSystem& newImageSystem)
    : bufferManager{newBufferManager}, imageSystem{newImageSystem} {
  Log::trace("Creating ResourceSystem");
}

ResourceSystem::~ResourceSystem() {
  Log::trace("Destroying Resource Facade");
}

auto ResourceSystem::createBuffer(const BufferCreateInfo& info) -> BufferHandle {
  return bufferManager.registerBuffer(info);
}

auto ResourceSystem::createPerFrameBuffer(const BufferCreateInfo& info, uint32_t frameCount)
    -> PerFrameBufferHandles {
  return bufferManager.registerPerFrameBuffer(info, frameCount);
}

auto ResourceSystem::getBufferHandle(LogicalBufferHandle logicalHandle, size_t frameIndex)
    -> BufferHandle {
  return bufferManager.resolve(logicalHandle, frameIndex);
}

auto ResourceSystem::getBuffer(LogicalBufferHandle logicalHandle, size_t frameIndex) -> Buffer& {
  const auto bufferHandle = bufferManager.resolve(logicalHandle, frameIndex);
  return bufferManager.getBuffer(bufferHandle);
}

auto ResourceSystem::getBuffer(BufferHandle handle) -> Buffer& {
  return bufferManager.getBuffer(handle);
}

auto ResourceSystem::createImage(const ImageSpec& imageSpec) -> ImageHandle {
  return imageSystem.createImage(imageSpec);
}

auto ResourceSystem::createImageView(const ImageViewSpec& imageViewSpec) -> ImageViewHandle {
  return imageSystem.createImageView(imageViewSpec);
}

auto ResourceSystem::resolveImageHandle(LogicalImageHandle logicalHandle, uint32_t frameIndex)
    -> ImageHandle {
  return imageSystem.resolveImageHandle(logicalHandle, frameIndex);
}

auto ResourceSystem::getImage(LogicalImageHandle logicalHandle, size_t frameIndex) -> const Image& {
  return imageSystem.getImage(logicalHandle, frameIndex);
}

auto ResourceSystem::getImage(ImageHandle handle) -> const Image& {
  return imageSystem.getImage(handle);
}

auto ResourceSystem::getImageView(LogicalImageViewHandle logicalHandle, uint32_t frameIndex)
    -> const ImageView& {
  return imageSystem.getImageView(logicalHandle, frameIndex);
}

auto ResourceSystem::getImageView(ImageViewHandle handle) -> const ImageView& {
  return imageSystem.getImageView(handle);
}

}
