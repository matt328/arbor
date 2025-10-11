#pragma once

#include "core/ImageHandle.hpp"
#include <optional>
#include <string>
#include <vulkan/vulkan_core.h>

namespace arb {
struct ImageViewSpec {
  ImageHandle image{};
  VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
  VkFormat format = VK_FORMAT_UNDEFINED;
  VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  uint32_t baseMipLevel = 0;
  uint32_t levelCount = 1;
  uint32_t baseArrayLayer = 0;
  uint32_t layerCount = 1;
  std::optional<std::string> debugName = std::nullopt;

  auto operator==(const ImageViewSpec& other) const -> bool {
    return image == other.image && viewType == other.viewType && format == other.format &&
           aspectMask == other.aspectMask && baseMipLevel == other.baseMipLevel &&
           levelCount == other.levelCount && baseArrayLayer == other.baseArrayLayer &&
           layerCount == other.layerCount && debugName == other.debugName;
  }
};
}
