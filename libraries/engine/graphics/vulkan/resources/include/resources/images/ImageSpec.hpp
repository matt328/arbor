#pragma once

#include "resources/ResizePolicy.hpp"

#include <optional>
#include <string>
#include <vulkan/vulkan_core.h>

namespace arb {

enum class ImageLifetime : uint8_t {
  Transient = 0,
  Persistent,
  Swapchain
};

struct ImageSpec {
  ImageLifetime imageLifetime = ImageLifetime::Transient;
  VkFormat format{};
  VkExtent2D extent{};
  VkImageUsageFlags usageFlags{};
  uint32_t mipLevels = 1;
  uint32_t layers = 1;
  ResizePolicy resizePolicy = ResizePolicy::Fixed;
  VkSampleCountFlags samples{VK_SAMPLE_COUNT_1_BIT};
  bool transient{false};
  std::optional<std::string> debugName = std::nullopt;

  auto operator==(const ImageSpec& other) const -> bool {
    return imageLifetime == other.imageLifetime && format == other.format &&
           extent.height == other.extent.height && extent.width == other.extent.width &&
           usageFlags == other.usageFlags && mipLevels == other.mipLevels &&
           layers == other.layers && resizePolicy == other.resizePolicy &&
           samples == other.samples && transient == other.transient && debugName == other.debugName;
  }
};

struct SpecHash {
  auto operator()(const ImageSpec& spec) const noexcept -> std::size_t {
    std::size_t h = 0;
    // combine like boost::hash_combine
    auto combine = [&](auto v) {
      h ^= std::hash<decltype(v)>{}(v) + 0x9e3779b9 + (h << 6) + (h >> 2);
    };
    combine(spec.imageLifetime);
    combine(spec.format);
    combine(spec.extent.width);
    combine(spec.extent.height);
    combine(spec.usageFlags);
    combine(spec.mipLevels);
    combine(spec.layers);
    combine(static_cast<int>(spec.resizePolicy));
    return h;
  }
};

}
