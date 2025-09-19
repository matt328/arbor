#pragma once

#include <memory>

#include "CompositionDispatcher.hpp"
#include "framegraph/ComponentIds.hpp"

namespace arb {

class ShaderBindingProxy {};

class CompositionDispatcher;
class ResourceFacade;

struct CompositionCreateInfo {
  DispatcherId id;
  ShaderBindingProxy& defaultShaderBinding;

  auto create(ResourceFacade& resourceFacade) const -> std::unique_ptr<CompositionDispatcher> {
    return std::make_unique<CompositionDispatcher>(resourceFacade, defaultShaderBinding);
  }
};

}
