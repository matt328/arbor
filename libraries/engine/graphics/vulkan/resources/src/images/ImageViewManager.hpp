#pragma once

#include "bk/NonCopyMove.hpp"
#include "resources/images/ImageView.hpp"
#include "core/Device.hpp"
#include "engine/common/HandleGenerator.hpp"
#include "core/ImageHandle.hpp"
#include "vulkan/vulkan_core.h"

namespace arb {

class ImageManager;

class ImageViewManager : NonCopyableMovable {
public:
  explicit ImageViewManager(Device& newDevice, ImageManager& newImageManager);
  ~ImageViewManager();

  auto createImageView(const VkImageViewCreateInfo& createInfo,
                       ImageHandle imageHandle,
                       const std::optional<std::string>& name) -> ImageViewHandle;
  auto destroyImageView(ImageViewHandle handle) -> void;
  auto getImageView(ImageViewHandle handle) -> const ImageView&;

private:
  Device& device;
  ImageManager& imageManager;

  HandleGenerator<ImageViewTag> handleGenerator;
  std::unordered_map<ImageViewHandle, std::unique_ptr<ImageView>> imageViewMap;
};
}
