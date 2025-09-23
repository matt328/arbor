#pragma once

#include "dispatcher/DispatcherHandles.hpp"
#include "framegraph/ComponentIds.hpp"
#include "framegraph/IGraphInfoProvider.hpp"

namespace arb {

class Frame;
class IDispatchContext;

class IRenderPass : public IGraphInfoProvider {
public:
  IRenderPass() = default;
  virtual ~IRenderPass() override = default;

  [[nodiscard]] virtual auto getId() const -> PassId = 0;
  virtual auto execute(Frame* frame, VkCommandBuffer cmdBuffer) -> void = 0;
  virtual auto registerDispatchContext(DispatcherHandle handle) -> void = 0;
};

}
