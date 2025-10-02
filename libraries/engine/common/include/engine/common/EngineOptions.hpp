#pragma once

#include "RenderSurfaceState.hpp"

namespace arb {

struct EngineOptions {
  bool debugEnabled{false};
  RenderSurfaceState initialSurfaceState{};
};

}
