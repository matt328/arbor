#pragma once

#include <format>
#include <vulkan/vulkan_core.h>
#include <vulkan/vk_enum_string_helper.h>

namespace arb {
struct BufferBarrierPrecursor {
  std::string alias;
  VkAccessFlags2 accessFlags;
  VkPipelineStageFlags2 stageFlags;
};
}

template <>
struct std::formatter<arb::BufferBarrierPrecursor> {
  // NOLINTNEXTLINE
  constexpr auto parse(std::format_parse_context& ctx) {
    return ctx.begin();
  }

  // NOLINTNEXTLINE
  auto format(const arb::BufferBarrierPrecursor& bbp, std::format_context& ctx) const {
    return std::format_to(ctx.out(),
                          "BufferBarrierPrecursor {{ alias={}, accessFlags={}, stageFlags={} }}",
                          bbp.alias,
                          string_VkAccessFlags2(bbp.accessFlags),
                          string_VkPipelineStageFlags2(bbp.stageFlags));
  }
};
