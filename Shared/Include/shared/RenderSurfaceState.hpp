#pragma once

#include <cmath>
#include <format>
#include "Size.hpp"

namespace Arbor {

struct RenderSurfaceState {
  Size swapchainExtent;
  float renderScale;

  [[nodiscard]] auto renderSize() const {
    return Size{.width = static_cast<uint32_t>(
                    std::lround(static_cast<float>(swapchainExtent.width) * renderScale)),
                .height = static_cast<uint32_t>(
                    std::lround(static_cast<float>(swapchainExtent.height) * renderScale))};
  }
};

}

template <>
struct std::formatter<arb::RenderSurfaceState> {
  // No custom format specifiers, so just return the end of the parse context.
  constexpr auto parse(std::format_parse_context& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const arb::RenderSurfaceState& state, FormatContext& ctx) const {
    const auto render = state.renderSize();
    return std::format_to(ctx.out(),
                          "RenderSurfaceState(swapchainExtent={{{}x{}}}, renderScale={}, "
                          "renderSize={{{}x{}}})",
                          state.swapchainExtent.width,
                          state.swapchainExtent.height,
                          state.renderScale,
                          render.width,
                          render.height);
  }
};
