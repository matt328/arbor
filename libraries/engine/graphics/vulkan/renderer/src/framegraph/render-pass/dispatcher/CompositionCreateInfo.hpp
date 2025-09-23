#pragma once

#include <memory>

#include "CompositionDispatcher.hpp"
#include "framegraph/ComponentIds.hpp"

namespace arb {

class ShaderBindingProxy {};

class CompositionDispatcher;
class ResourceSystem;

struct CompositionCreateInfo {
  DispatcherId id;
  ShaderBindingProxy& defaultShaderBinding;

  auto create(ResourceSystem& resourceSystem) const -> std::unique_ptr<CompositionDispatcher> {
    return std::make_unique<CompositionDispatcher>(resourceSystem, defaultShaderBinding);
  }
};

}
