#include "renderer/RenderContext.hpp"

#include "bk/Logger.hpp"

#include "core/Device.hpp"
#include "core/Swapchain.hpp"
#include "core/command-buffers/CommandBufferManager.hpp"
#include "resources/ResourceSystem.hpp"

#include "graphics/common/GraphicsOptions.hpp"

#include "framegraph/FrameGraph.hpp"

#include "FrameManager.hpp"
#include "FrameRenderer.hpp"
#include "PerFrameUploader.hpp"
#include "framegraph/AliasRegistry.hpp"

namespace arb {

RenderContext::RenderContext(const GraphicsOptions& graphicsOptions,
                             Device& newDevice,
                             Swapchain& newSwapchain,
                             IStateBuffer<SimState>& simStateBuffer,
                             GeometryHandleMapper& newGeometryHandleMapper,
                             CommandBufferManager& newCommandBufferManager,
                             ResourceSystem& newResourceSystem,
                             PipelineManager& newPipelineManager)
    : device{newDevice}, swapchain{newSwapchain}, pipelineManager{newPipelineManager} {
  Log::trace("Creating RenderContext");
  frameManager = std::make_unique<FrameManager>(graphicsOptions, device, swapchain);
  perFrameUploader = std::make_unique<PerFrameUploader>(simStateBuffer, newGeometryHandleMapper);
  aliasRegistry = std::make_unique<AliasRegistry>(newResourceSystem);

  frameGraph =
      std::make_unique<FrameGraph>(newCommandBufferManager, newPipelineManager, *aliasRegistry);

  frameRenderer = std::make_unique<FrameRenderer>(device,
                                                  swapchain,
                                                  *frameManager,
                                                  *perFrameUploader,
                                                  *aliasRegistry,
                                                  *frameGraph);
}

RenderContext::~RenderContext() {
  Log::trace("Destroying RenderContext");
}

void RenderContext::renderNextFrame() {
  frameRenderer->renderNextFrame();
}

}
