#pragma once

#include <string>

#include "core/shader-binding/Handles.hpp"

namespace arb {

struct CompositionCreateInfo {
  std::string colorImage;
  std::string swapchainImage;
  ShaderBindingHandle defaultShaderBinding;
  DSLayoutHandle defaultDSLayout;
};

}
