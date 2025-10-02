#pragma once

#include <format>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_core.h>

namespace arb {

struct ImageUseDescription {
  VkAccessFlags2 accessFlags{};
  VkPipelineStageFlags2 stageFlags{};
  VkImageLayout imageLayout{};
  VkImageAspectFlags aspectFlags{};

  friend auto operator==(const ImageUseDescription& lhs, const ImageUseDescription& rhs) noexcept
      -> bool {
    return lhs.accessFlags == rhs.accessFlags && lhs.stageFlags == rhs.stageFlags &&
           lhs.imageLayout == rhs.imageLayout && lhs.aspectFlags == rhs.aspectFlags;
  }

  friend auto operator!=(const ImageUseDescription& lhs, const ImageUseDescription& rhs) noexcept
      -> bool {
    return !(lhs == rhs);
  }
};

}

template <>
struct std::formatter<arb::ImageUseDescription> : std::formatter<std::string> {
  auto format(const arb::ImageUseDescription& desc, std::format_context& ctx) const {
    auto accessFlagsStr = string_VkAccessFlags2(desc.accessFlags);
    auto stageFlagsStr = string_VkPipelineStageFlags2(desc.stageFlags);
    const auto* layoutStr = string_VkImageLayout(desc.imageLayout);
    auto aspectStr = string_VkImageAspectFlags(desc.aspectFlags);

    return std::formatter<std::string>::format(
        std::format("ImageUseDescription(accessFlags={}, stageFlags={}, "
                    "imageLayout={}, aspectFlags={})",
                    accessFlagsStr,
                    stageFlagsStr,
                    layoutStr,
                    aspectStr),
        ctx);
  }
};
