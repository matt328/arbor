#include "renderer/RenderContext.hpp"

#include "bk/Logger.hpp"

#include "core/Device.hpp"
#include "core/Swapchain.hpp"
#include "core/command-buffers/CommandBufferManager.hpp"
#include "engine/common/EngineOptions.hpp"
#include "resources/ResourceSystem.hpp"

#include "framegraph/FrameGraph.hpp"

#include "FrameManager.hpp"
#include "FrameRenderer.hpp"
#include "PerFrameUploader.hpp"
#include "framegraph/AliasRegistry.hpp"
#include "engine/common/ResizeEvent.hpp"

namespace arb {

// Pass eventQueue in here and subscribe to resize events and call method on frameRenderer and let
// it call resize on frameGraph
RenderContext::RenderContext(const EngineOptions& options, const RenderContextDeps& deps)
    : device{deps.device}, swapchain{deps.swapchain}, pipelineManager{deps.pipelineManager} {
  Log::trace("Creating RenderContext");

  frameManager = std::make_unique<FrameManager>(options, device, swapchain);
  perFrameUploader =
      std::make_unique<PerFrameUploader>(deps.simStateBuffer, deps.geometryHandleMapper);
  aliasRegistry = std::make_unique<AliasRegistry>(
      AliasRegistryDeps{.resourceSystem = deps.resourceSystem, .swapchain = swapchain});

  frameGraph = std::make_unique<FrameGraph>(
      FrameGraphDeps{.commandBufferManager = deps.commandBufferManager,
                     .pipelineManager = deps.pipelineManager,
                     .aliasRegistry = *aliasRegistry},
      FrameGraphConfig{.initialSurfaceState = options.initialSurfaceState});

  frameRenderer = std::make_unique<FrameRenderer>(
      FrameRendererDeps{.device = device,
                        .swapchain = swapchain,
                        .frameManager = *frameManager,
                        .perFrameUploader = *perFrameUploader,
                        .aliasRegistry = *aliasRegistry,
                        .frameGraph = *frameGraph},
      FrameRendererCreateInfo{.renderSurfaceState = options.initialSurfaceState});

  deps.eventQueue.subscribe<ResizeEvent>([&](auto event) { frameRenderer->resize({}); });
}

RenderContext::~RenderContext() {
  Log::trace("Destroying RenderContext");
}

void RenderContext::renderNextFrame() {
  frameRenderer->renderNextFrame();
}

}
