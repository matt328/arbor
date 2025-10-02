#pragma once

#include <cstdint>
#include <format>
#include <string_view>

namespace arb {

enum class ResizePolicy : uint8_t {
  Fixed = 0,
  MatchSwapchain,
  SwapchainImage,
  RenderScale,
  Custom
};

}

template <>
struct std::formatter<arb::ResizePolicy> : std::formatter<std::string_view> {
  auto format(arb::ResizePolicy policy, std::format_context& ctx) const {
    const char* name = nullptr;
    switch (policy) {
      case arb::ResizePolicy::Fixed:
        name = "Fixed";
        break;
      case arb::ResizePolicy::MatchSwapchain:
        name = "MatchSwapchain";
        break;
      case arb::ResizePolicy::SwapchainImage:
        name = "SwapchainImage";
        break;
      case arb::ResizePolicy::RenderScale:
        name = "RenderScale";
        break;
      case arb::ResizePolicy::Custom:
        name = "Custom";
        break;
      default:
        name = "Unknown";
        break;
    }
    return std::formatter<std::string_view>::format(name, ctx);
  }
};
