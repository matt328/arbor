#pragma once

#include <vulkan/vulkan_core.h>
#include <vulkan/vk_enum_string_helper.h>

#include "common/AccessMode.hpp"

namespace arb {

struct ImageBarrierPrecursor {
  std::string alias;
  bool isWriteAccess{false};
  VkAccessFlags2 accessFlags;
  VkPipelineStageFlags2 stageFlags;
  VkImageLayout layout;
  VkImageAspectFlags aspectFlags;
};

}

template <>
struct std::formatter<arb::ImageBarrierPrecursor> {
  // NOLINTNEXTLINE
  constexpr auto parse(std::format_parse_context& ctx) {
    return ctx.begin();
  }

  // NOLINTNEXTLINE
  auto format(const arb::ImageBarrierPrecursor& ibp, std::format_context& ctx) const {
    return std::format_to(
        ctx.out(),
        "ImageBarrierPrecursor {{\n        alias={}\n        isWriteAccess={}\n     "
        "   accessFlags={}\n        stageFlags={}\n        "
        "layout={}\n        aspectFlags={}\n      }}",
        ibp.alias,
        ibp.isWriteAccess,
        string_VkAccessFlagBits2(ibp.accessFlags),
        string_VkPipelineStageFlagBits2(ibp.stageFlags),
        string_VkImageLayout(ibp.layout),
        string_VkImageAspectFlags(ibp.aspectFlags));
  }
};
