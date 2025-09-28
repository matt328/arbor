#pragma once

#include "framegraph/ComponentIds.hpp"
#include "pass/reqs/PassDescription.hpp"

namespace arb {

class Frame;
class IDispatchContext;

class IRenderPass {
public:
  IRenderPass() = default;
  virtual ~IRenderPass() = default;

  [[nodiscard]] virtual auto getId() const -> PassId = 0;
  virtual void execute(Frame* frame, VkCommandBuffer cmdBuffer) = 0;
  [[nodiscard]] virtual auto getDescription() const -> PassDescription = 0;
};

}
