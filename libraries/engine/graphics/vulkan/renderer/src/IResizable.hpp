#pragma once

#include "engine/common/RenderSurfaceState.hpp"

namespace arb {

class IResizable {
public:
  virtual ~IResizable() = default;
  virtual void resize(const RenderSurfaceState& newState) = 0;
};

}
