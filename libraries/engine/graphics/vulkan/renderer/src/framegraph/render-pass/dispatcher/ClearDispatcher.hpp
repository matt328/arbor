#pragma once

#include "IDispatcher.hpp"

namespace arb {

class ClearDispatcher : public IDispatcher {
public:
  explicit ClearDispatcher(AliasRegistry& newAliasRegistry);
  virtual ~ClearDispatcher();

  void bindAlias(std::string alias, std::string logicalName) override;
  auto dispatch(const Frame* frame, VkPipelineLayout pipelineLayout, VkCommandBuffer commandBuffer)
      -> void override;
  auto getPushConstantSize() -> size_t override;
  auto requirements() -> DispatcherRequirements override;
};

}
