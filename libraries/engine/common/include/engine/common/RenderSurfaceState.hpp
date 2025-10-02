#pragma once

#include <cmath>
#include "Size.hpp"

namespace arb {

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
