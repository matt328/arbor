#pragma once

#include "engine/common/Handle.hpp"
#include "framegraph/ComponentIds.hpp"
#include "framegraph/passes/IGraphInfoProvider.hpp"

namespace arb {

class Frame;
class IDispatchContext;

class IRenderPass : public IGraphInfoProvider {
public:
  IRenderPass() = default;
  virtual ~IRenderPass() override = default;

  IRenderPass(const IRenderPass&) = default;
  IRenderPass(IRenderPass&&) = delete;
  auto operator=(const IRenderPass&) -> IRenderPass& = default;
  auto operator=(IRenderPass&&) -> IRenderPass& = delete;

  [[nodiscard]] virtual auto getId() const -> PassId = 0;
  virtual auto execute(Frame* frame, VkCommandBuffer cmdBuffer) -> void = 0;
  virtual auto registerDispatchContext(Handle<IDispatchContext> handle) -> void = 0;
};

}
