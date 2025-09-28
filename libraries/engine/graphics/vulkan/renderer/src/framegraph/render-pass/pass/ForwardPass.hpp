#pragma once

#include <vector>

#include "core/pipeline/PipelineManager.hpp"
#include "framegraph/AliasRegistry.hpp"
#include "framegraph/render-pass/IRenderPass.hpp"
#include "reqs/DispatchBinding.hpp"

namespace arb {

struct ForwardPassContext {
  AliasRegistry& aliasRegistry;
  PipelineManager& pipelineManager;
};

class ForwardPass : public IRenderPass {
public:
  ~ForwardPass();

  [[nodiscard]] auto getId() const -> PassId override;
  auto execute(Frame* frame, VkCommandBuffer cmdBuffer) -> void override;
  [[nodiscard]] auto getDescription() const -> PassDescription override;

  static auto create(const ForwardPassContext& ctx) -> std::unique_ptr<ForwardPass>;

private:
  struct ForwardPassConfig {
    AliasRegistry& aliasRegistry;
    PipelineManager& pipelineManager;
    PipelineUnitHandle pipelineHandle;
  };

  explicit ForwardPass(const ForwardPassConfig& config);

  AliasRegistry& aliasRegistry;
  PipelineManager& pipelineManager;

  PipelineUnitHandle pipelineHandle;
  std::string colorImageAlias;
  std::string depthImageAlias;

  std::vector<std::unique_ptr<IDispatcher>> dispatchers;
  std::unordered_map<std::string, std::string> bindingMap;
};

}
