#pragma once

#include "RenderSurfaceState.hpp"

namespace Arbor {

struct EngineOptions {
  bool debugEnabled{false};
  RenderSurfaceState initialSurfaceState{};
};

}
