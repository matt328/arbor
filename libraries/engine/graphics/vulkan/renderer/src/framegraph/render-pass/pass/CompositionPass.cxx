#include "CompositionPass.hpp"

#include "bk/Log.hpp"

namespace arb {

CompositionPass::CompositionPass(const CompositionPassDeps& deps,
                                 const CompositionPassConfig& config)
    : aliasRegistry{deps.aliasRegistry}, pipelineManager{deps.pipelineManager} {
  LOG_TRACE_L1(Log::Renderer, "Creating CompositionPass");
}

CompositionPass::~CompositionPass() {
  LOG_TRACE_L1(Log::Renderer, "Destroying CompositionPass");
}

auto CompositionPass::getId() const -> PassId {
  return PassId::Composition;
}

void CompositionPass::execute(Frame* frame, VkCommandBuffer cmdBuffer) {
}

auto CompositionPass::getDescription() const -> PassDescription {
}

}
