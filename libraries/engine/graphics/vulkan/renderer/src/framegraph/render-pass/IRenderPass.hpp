#pragma once

#include "engine/common/Handle.hpp"
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
  virtual auto registerDispatchContext(Handle<IDispatchContext> handle) -> void = 0;
};

}
