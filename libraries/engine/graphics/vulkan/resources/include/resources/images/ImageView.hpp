#pragma once

#include "bk/NonCopyMove.hpp"
#include "core/Device.hpp"
#include "core/ImageHandle.hpp"

#include <string>
#include <vulkan/vulkan_core.h>

namespace arb {

class ImageView : NonCopyableMovable {
public:
  ImageView(Device& newDevice,
            const VkImageViewCreateInfo& createInfo,
            ImageHandle imageHandle,
            const std::optional<std::string>& name = std::nullopt);
  ~ImageView();

  operator VkImageView() const {
    return handle;
  }

  auto getSourceImageHandle() -> ImageHandle {
    return sourceImage;
  }

private:
  Device& device;
  VkImageView handle;
  ImageHandle sourceImage;
  std::string debugName;
};

}
