#pragma once

#include "IDispatcher.hpp"

namespace arb {

class ShaderBindingProxy;

class CompositionDispatcher : public IDispatcher {
public:
  CompositionDispatcher(ResourceSystem& newResourceFacade,
                        ShaderBindingProxy& newShaderBindingProxy);
  ~CompositionDispatcher() override;

  auto bind(const Frame* frame, VkCommandBuffer commandBuffer, VkPipelineLayout layout)
      -> void override;

  auto dispatch(const Frame* frame, VkCommandBuffer commandBuffer) -> void override;

  auto getPushConstantSize() -> size_t override;

  [[nodiscard]] auto getGraphInfo() const -> PassGraphInfo override;

private:
  ResourceSystem& resourceSystem;
  ShaderBindingProxy& shaderBindingProxy;
};

}
