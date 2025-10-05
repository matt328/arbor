#pragma once

#include "core/pipeline/PipelineManager.hpp"
#include "engine/common/RenderSurfaceState.hpp"
#include "framegraph/AliasRegistry.hpp"
#include "framegraph/render-pass/IRenderPass.hpp"

namespace arb {

struct ForwardPassDeps {
  AliasRegistry& aliasRegistry;
  PipelineManager& pipelineManager;
};

struct ForwardPassConfig {
  RenderSurfaceState initialSurfaceState;
};

class ForwardPass : public IRenderPass {
public:
  explicit ForwardPass(const ForwardPassDeps& deps, const ForwardPassConfig& config);
  ~ForwardPass();

  [[nodiscard]] auto getId() const -> PassId override;
  auto execute(Frame* frame, VkCommandBuffer cmdBuffer) -> void override;
  [[nodiscard]] auto getDescription() const -> PassDescription override;

  void resize(const RenderSurfaceState& newState) override;

private:
  AliasRegistry& aliasRegistry;
  PipelineManager& pipelineManager;

  PipelineUnitHandle pipelineHandle;
  std::string colorImageAlias;
  std::string depthImageAlias;

  RenderSurfaceState surfaceState;
};

}
