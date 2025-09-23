#pragma once

#include <vector>

#include "core/pipeline/PipelineManager.hpp"
#include "framegraph/AliasRegistry.hpp"
#include "framegraph/render-pass/IRenderPass.hpp"
#include "framegraph/render-pass/dispatcher/DispatcherHandles.hpp"
#include "resources/ResourceSystem.hpp"
#include "framegraph/render-pass/dispatcher/DispatcherFactory.hpp"

namespace arb {

struct ForwardPassContext {
  AliasRegistry& aliasRegistry;
  ResourceSystem& resourceSystem;
  DispatcherFactory& dispatcherFactory;
  PipelineManager& pipelineManager;
};

struct ForwardPassConfig {
  PipelineUnitHandle pipelineHandle;
  std::vector<DispatcherHandle> dispatcherHandles;
  std::string colorImage;
  std::string depthImage;
};

class ForwardPass : public IRenderPass {
public:
  ForwardPass(const ForwardPassContext& ctx, const ForwardPassConfig& config);
  ~ForwardPass();

  [[nodiscard]] auto getId() const -> PassId override;
  auto execute(Frame* frame, VkCommandBuffer cmdBuffer) -> void override;
  auto registerDispatchContext(DispatcherHandle handle) -> void override;

private:
  AliasRegistry& aliasRegistry;
  ResourceSystem& resourceSystem;
  DispatcherFactory& dispatcherFactory;
  PipelineManager& pipelineManager;

  PipelineUnitHandle pipelineHandle;
  std::vector<DispatcherHandle> dispatcherHandles;
  std::string colorImage;
  std::string depthImage;
};

}
