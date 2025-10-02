#pragma once

#include <cstdint>
#include <format>
#include <string_view>

namespace arb {

enum class ImageLifetime : uint8_t {
  Persistent = 0,
  Transient,
  Swapchain
};

}

template <>
struct std::formatter<arb::ImageLifetime> : std::formatter<std::string_view> {
  auto format(arb::ImageLifetime lifetime, std::format_context& ctx) const {
    std::string_view name = "Unknown";
    switch (lifetime) {
      case arb::ImageLifetime::Persistent:
        name = "Persistent";
        break;
      case arb::ImageLifetime::Transient:
        name = "Transient";
        break;
      case arb::ImageLifetime::Swapchain:
        name = "Swapchain";
        break;
    }
    return std::formatter<std::string_view>::format(name, ctx);
  }
};
