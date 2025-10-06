#pragma once

#include <format>
#include <string>
#include <vulkan/vulkan_core.h>
#include <vulkan/vk_enum_string_helper.h>

namespace arb {

struct BufferUseDescription {
  std::string alias;
  VkAccessFlags2 accessFlags;
  VkPipelineStageFlags2 stageFlags;

  friend auto operator==(const BufferUseDescription& lhs, const BufferUseDescription& rhs) noexcept
      -> bool {
    return lhs.accessFlags == rhs.accessFlags && lhs.stageFlags == rhs.stageFlags;
  }

  friend auto operator!=(const BufferUseDescription& lhs, const BufferUseDescription& rhs) noexcept
      -> bool {
    return !(lhs == rhs);
  }
};

}

template <>
struct std::formatter<arb::BufferUseDescription> : std::formatter<std::string> {
  auto format(const arb::BufferUseDescription& desc, std::format_context& ctx) const {
    auto accessFlagsStr = string_VkAccessFlags2(desc.accessFlags);
    auto stageFlagsStr = string_VkPipelineStageFlags2(desc.stageFlags);

    return std::formatter<std::string>::format(
        std::format("ImageUseDescription(accessFlags={}, stageFlags={})",
                    accessFlagsStr,
                    stageFlagsStr),
        ctx);
  }
};
