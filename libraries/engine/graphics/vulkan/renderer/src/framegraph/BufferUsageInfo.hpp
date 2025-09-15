#pragma once

#include <unordered_set>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_core.h>

#include "framegraph/ResourceAliases.hpp"

namespace arb {

struct BufferUsageInfo {
  BufferAliasVariant alias;
  VkAccessFlags2 accessFlags;
  VkPipelineStageFlags2 stageFlags;
  VkDeviceSize offset = 0;
  VkDeviceSize size = VK_WHOLE_SIZE;

  auto operator==(const BufferUsageInfo& other) const -> bool {
    return alias == other.alias && accessFlags == other.accessFlags &&
           stageFlags == other.stageFlags && offset == other.offset && size == other.size;
  }
};

struct BufferUsageInfoHash {
  auto operator()(const BufferUsageInfo& b) const -> std::size_t {
    std::size_t h1 = std::visit(
        [](auto&& alias) {
          using T = std::decay_t<decltype(alias)>;
          std::size_t tag = std::is_same_v<T, BufferAlias> ? 0 : 1;
          return (std::hash<T>{}(alias) << 1) | tag;
        },
        b.alias);

    std::size_t h2 = std::hash<uint64_t>{}(static_cast<uint64_t>(b.accessFlags));
    std::size_t h3 = std::hash<uint64_t>{}(static_cast<uint64_t>(b.stageFlags));
    std::size_t h4 = std::hash<VkDeviceSize>{}(b.offset);
    std::size_t h5 = std::hash<VkDeviceSize>{}(b.size);

    return (((((h1 ^ (h2 << 1)) ^ (h3 << 1)) ^ (h4 << 1)) ^ (h5 << 1)));
  }
};

using BufferUsageInfoSet = std::unordered_set<BufferUsageInfo, BufferUsageInfoHash>;

}

template <>
struct std::formatter<arb::BufferUsageInfo> {
  constexpr auto parse(format_parse_context& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const arb::BufferUsageInfo& info, FormatContext& ctx) {
    return format_to(ctx.out(),
                     "{{ alias: {}, access: {}, stage: {}, offset: {}, size: {} }}",
                     to_string(info.alias),
                     string_VkAccessFlags2(info.accessFlags),
                     string_VkPipelineStageFlags2(info.stageFlags),
                     info.offset,
                     info.size == VK_WHOLE_SIZE ? "VK_WHOLE_SIZE" : std::to_string(info.size));
  }
};
