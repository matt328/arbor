#pragma once

#include <cstdint>

namespace arb {
enum class ResizePolicy : uint8_t {
  Fixed = 0,
  MatchSwapchain,
  SwapchainImage,
  RenderScale,
  Custom
};
}
