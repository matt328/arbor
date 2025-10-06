#pragma once

#include "framegraph/render-pass/IRenderPass.hpp"
#include "core/pipeline/PipelineManager.hpp"
#include "framegraph/AliasRegistry.hpp"

namespace arb {

struct CompositionPassDeps {
  AliasRegistry& aliasRegistry;
  PipelineManager& pipelineManager;
};

struct CompositionPassConfig {};

class CompositionPass : public IRenderPass {
public:
  explicit CompositionPass(const CompositionPassDeps& deps, const CompositionPassConfig& config);
  ~CompositionPass() override;

  [[nodiscard]] auto getId() const -> PassId override;
  auto execute(Frame* frame, VkCommandBuffer cmdBuffer) -> void override;
  [[nodiscard]] auto getDescription() const -> PassDescription override;

private:
  AliasRegistry& aliasRegistry;
  PipelineManager& pipelineManager;

  PipelineUnitHandle pipelineHandle;
};

}
