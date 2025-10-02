#pragma once

#include "ResizePolicy.hpp"
#include "ImageLifetime.hpp"

#include <format>
#include <optional>
#include <string>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_core.h>

namespace arb {

struct ImageCreateDescription {
  VkFormat format{};
  VkExtent2D extent{};
  VkImageUsageFlags usage{};
  uint32_t mipLevels{1};
  uint32_t arrayLayers{1};
  VkSampleCountFlagBits samples{VK_SAMPLE_COUNT_1_BIT};
  ResizePolicy resizePolicy{ResizePolicy::MatchSwapchain};
  ImageLifetime imageLifetime{ImageLifetime::Persistent};
  std::optional<std::string> debugName;

  // Equality operators
  friend auto operator==(const ImageCreateDescription& lhs, const ImageCreateDescription& rhs)
      -> bool {
    return lhs.format == rhs.format && lhs.extent.width == rhs.extent.width &&
           lhs.extent.height == rhs.extent.height && lhs.usage == rhs.usage &&
           lhs.mipLevels == rhs.mipLevels && lhs.arrayLayers == rhs.arrayLayers &&
           lhs.samples == rhs.samples && lhs.resizePolicy == rhs.resizePolicy &&
           lhs.imageLifetime == rhs.imageLifetime;
  }

  friend auto operator!=(const ImageCreateDescription& lhs, const ImageCreateDescription& rhs)
      -> bool {
    return !(lhs == rhs);
  }
};

}
template <>
struct std::formatter<arb::ImageCreateDescription> : std::formatter<std::string> {
  auto format(const arb::ImageCreateDescription& desc, auto& ctx) const {
    return std::formatter<std::string>::format(
        std::format("ImageCreateDescription(format={}, extent=({}x{}), usage={}, "
                    "mipLevels={}, arrayLayers={}, samples={}, resizePolicy={}, imageLifetime={})",
                    string_VkFormat(desc.format),
                    desc.extent.width,
                    desc.extent.height,
                    string_VkImageUsageFlags(desc.usage),
                    desc.mipLevels,
                    desc.arrayLayers,
                    string_VkSampleCountFlagBits(desc.samples),
                    desc.resizePolicy,
                    desc.imageLifetime),
        ctx);
  }
};
