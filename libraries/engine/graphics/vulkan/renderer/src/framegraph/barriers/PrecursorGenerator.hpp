#pragma once

#include <memory>
#include <vector>

#include "framegraph/barriers/BarrierPrecursorPlan.hpp"
#include "framegraph/render-pass/IRenderPass.hpp"

namespace arb {

static inline auto generatePrecursorPlan(const std::vector<std::unique_ptr<IRenderPass>>& passes)
    -> BarrierPrecursorPlan {
  BarrierPrecursorPlan result{};

  for (const auto& pass : passes) {
    const auto& passId = pass->getId();
    const auto info = pass->getDescription();

    // Log->trace("Generated BarrierPrecursorPlan:\n{}", result);
  }
  return result;
}
}
