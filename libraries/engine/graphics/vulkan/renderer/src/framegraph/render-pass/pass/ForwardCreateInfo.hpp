#pragma once

#include <string>
#include <vector>
#include "core/shader-binding/Handles.hpp"

namespace arb {

struct ForwardCreateInfo {
  std::string colorImage;
  std::string depthImage;
  std::vector<DSLayoutHandle> dsLayoutHandles;
};

}
