#include "ImageViewManager.hpp"

#include <cassert>

#include "core/Device.hpp"
#include "bk/Logger.hpp"
#include "ImageManager.hpp"
#include "engine/common/RenderSurfaceState.hpp"

namespace arb {

ImageViewManager::ImageViewManager(Device& newDevice, ImageManager& newImageManager)
    : device{newDevice}, imageManager{newImageManager} {
  Log::trace("Creating ImageViewManager");
}

ImageViewManager::~ImageViewManager() {
  Log::trace("Destroying ImageViewManager");
}

void ImageViewManager::resize(const RenderSurfaceState& newState) {
  for (const auto& [handle, imageView] : imageViewMap) {
    auto& image = imageManager.getImage(imageView->getSourceImageHandle());
    imageView->recreate(image);
  }
}

auto ImageViewManager::createImageView(const VkImageViewCreateInfo& createInfo,
                                       ImageHandle imageHandle,
                                       const std::optional<std::string>& name) -> ImageViewHandle {
  const auto handle = handleGenerator.requestHandle();
  imageViewMap.emplace(handle,
                       std::make_unique<ImageView>(device,
                                                   createInfo,
                                                   imageHandle,
                                                   name.value_or("Unnamed ImageView")));
  imageManager.incrementImageUse(imageHandle);
  return handle;
}

auto ImageViewManager::destroyImageView(ImageViewHandle handle) -> void {
  assert(imageViewMap.contains(handle));
  const auto imageHandle = imageViewMap.at(handle)->getSourceImageHandle();
  imageViewMap.erase(handle);
  imageManager.decrementImageUse(imageHandle);
}

auto ImageViewManager::getImageView(ImageViewHandle handle) -> const ImageView& {
  assert(imageViewMap.contains(handle));
  return *imageViewMap.at(handle);
}

}
