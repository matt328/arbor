#pragma once

#include "engine/common/RenderSurfaceState.hpp"
#include "framegraph/render-pass/IRenderPass.hpp"
#include "reqs/PassDescription.hpp"

namespace arb {

struct PresentPassConfig {
  RenderSurfaceState initialSurfaceState;
};

class PresentPass : public IRenderPass {
public:
  explicit PresentPass(const PresentPassConfig& config);
  ~PresentPass() override = default;

  [[nodiscard]] auto getId() const -> PassId override;
  auto execute(Frame* frame, VkCommandBuffer cmdBuffer) -> void override;
  [[nodiscard]] auto getDescription() const -> PassDescription override;

private:
  RenderSurfaceState surfaceState;
};

}
