#pragma once

#include "framegraph/ComponentIds.hpp"
#include "framegraph/IGraphInfoProvider.hpp"
#include "resources/ResourceFacade.hpp"

namespace arb {

class Frame;
class ResourceFacade;

class IDispatcher : public IGraphInfoProvider {
public:
  IDispatcher(DispatcherId newId, ResourceFacade& newResourceFacade)
      : id{newId}, resourceFacade{newResourceFacade} {
  }

  ~IDispatcher() override = default;

  virtual auto bind(const Frame* frame, VkCommandBuffer commandBuffer, VkPipelineLayout layout)
      -> void = 0;

  virtual auto dispatch(const Frame* frame, VkCommandBuffer commandBuffer) -> void = 0;

  virtual auto getPushConstantSize() -> size_t = 0;

protected:
  DispatcherId id;
  ResourceFacade& resourceFacade;
};

}
