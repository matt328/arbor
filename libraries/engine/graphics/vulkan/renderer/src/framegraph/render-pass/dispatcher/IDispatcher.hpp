#pragma once

#include "LogicalImageRequirement.hpp"
#include "framegraph/AliasRegistry.hpp"

namespace arb {

class Frame;
class ResourceSystem;

struct DispatcherRequirements {
  std::vector<LogicalImageRequirement> images;
};

class IDispatcher {
public:
  explicit IDispatcher(AliasRegistry& newAliasRegistry) : aliasRegistry{newAliasRegistry} {
  }

  virtual ~IDispatcher() = default;
  virtual void bindAlias(std::string alias, std::string logicalName) = 0;
  virtual auto dispatch(const Frame* frame,
                        VkPipelineLayout pipelineLayout,
                        VkCommandBuffer commandBuffer) -> void = 0;
  virtual auto getPushConstantSize() -> size_t = 0;
  virtual auto requirements() -> DispatcherRequirements = 0;

protected:
  AliasRegistry& aliasRegistry;
  std::unordered_map<std::string, std::string> logicalToAliasMap;
};

}
