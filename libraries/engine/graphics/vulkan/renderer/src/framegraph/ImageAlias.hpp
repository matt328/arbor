#pragma once

#include <cstdint>
#include <string>
#include <format>

namespace arb {

enum class ImageAlias : uint8_t {
  GeometryColorImage = 0,
  SwapchainImage,
  DepthImage,
  Count
};

// ImageAlias to_string
inline auto to_string(ImageAlias alias) -> std::string {
  switch (alias) {
    case ImageAlias::GeometryColorImage:
      return "GeometryColorImage";
    case ImageAlias::SwapchainImage:
      return "SwapchainImage";
    case ImageAlias::DepthImage:
      return "DepthImage";
    case ImageAlias::Count:
      return "Count";
    default:
      return "UnknownImageAlias";
  }
}

}

// ImageAlias Formatter
template <>
struct std::formatter<arb::ImageAlias> {
  // NOLINTNEXTLINE
  constexpr auto parse(format_parse_context& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(arb::ImageAlias alias, FormatContext& ctx) const {
    return std::format_to(ctx.out(), "{}", to_string(alias));
  }
};

// ImageAlias Hash
namespace std {
template <>
struct hash<arb::ImageAlias> {
  auto operator()(const arb::ImageAlias& alias) const noexcept -> std::size_t {
    using Underlying = std::underlying_type_t<arb::ImageAlias>;
    return std::hash<Underlying>{}(static_cast<Underlying>(alias));
  }
};
}
