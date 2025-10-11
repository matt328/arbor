#include "renderer/RenderContext.hpp"

#include "ResourceAliases.hpp"
#include "bk/Log.hpp"

#include "buffers/BufferSystem.hpp"
#include "images/ImageSystem.hpp"

#include "core/Device.hpp"
#include "core/Swapchain.hpp"
#include "core/command-buffers/CommandBufferManager.hpp"
#include "engine/common/EngineOptions.hpp"
#include "engine/common/RenderSurfaceState.hpp"
#include "geometry/GeometryStream.hpp"

#include "framegraph/FrameGraph.hpp"

#include "FrameManager.hpp"
#include "FrameRenderer.hpp"
#include "PerFrameUploader.hpp"
#include "framegraph/AliasRegistry.hpp"
#include "geometry/VirtualGeometryAllocator.hpp"
#include "transfer/TransferSystem.hpp"

namespace arb {

// Pass eventQueue in here and subscribe to resize events and call method on frameRenderer and let
// it call resize on frameGraph
RenderContext::RenderContext(const EngineOptions& options, const RenderContextDeps& deps)
    : device{deps.device}, swapchain{deps.swapchain}, pipelineManager{deps.pipelineManager} {
  LOG_TRACE_L1(Log::Renderer, "Creating RenderContext");

  frameManager = std::make_unique<FrameManager>(options, device, swapchain);

  transferSystem = std::make_unique<TransferSystem>(TransferSystemDeps{
      .device = device,
      .commandBufferManager = deps.commandBufferManager,
  });

  bufferSystem = std::make_unique<BufferSystem>(BufferSystemDeps{
      .device = device,
      .allocatorService = deps.allocatorService,
      .transferSystem = *transferSystem,
  });

  imageSystem = std::make_unique<ImageSystem>(device, deps.allocatorService);

  aliasRegistry = std::make_unique<AliasRegistry>(AliasRegistryDeps{.bufferSystem = *bufferSystem,
                                                                    .imageSystem = *imageSystem,
                                                                    .swapchain = swapchain});

  geometryStream = std::make_unique<GeometryStream>(*bufferSystem);
  registerGeometryAliases(*aliasRegistry, *geometryStream);

  geometryAllocator = std::make_unique<VirtualAllocationManager>(*geometryStream);

  perFrameUploader = std::make_unique<PerFrameUploader>(
      PerFrameUploaderDeps{
          .geometryHandleMapper = deps.geometryHandleMapper,
          .textureHandleMapper = deps.textureHandleMapper,
          .allocationManager = *geometryAllocator,
      },
      PerFrameUploaderConfig{.stateBuffer = deps.simStateBuffer});

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
  frameRenderer->setOnSwapchainResized(
      [&](RenderSurfaceState state) { imageSystem->resize(state); });
}

RenderContext::~RenderContext() {
  LOG_TRACE_L1(Log::Renderer, "Destroying RenderContext");
}

void RenderContext::registerGeometryAliases(AliasRegistry& aliasRegistry,
                                            GeometryStream& geometryStream) {
  aliasRegistry.registerBufferAlias(GlobalBufferAlias::Index, geometryStream.getIndexBuffer());
  aliasRegistry.registerBufferAlias(GlobalBufferAlias::Position,
                                    geometryStream.getPositionBuffer());
  aliasRegistry.registerBufferAlias(GlobalBufferAlias::Color, geometryStream.getColorBuffer());
  aliasRegistry.registerBufferAlias(GlobalBufferAlias::TexCoord,
                                    geometryStream.getTexCoordBuffer());
  aliasRegistry.registerBufferAlias(GlobalBufferAlias::Normal, geometryStream.getNormalBuffer());
  aliasRegistry.registerBufferAlias(GlobalBufferAlias::Animation,
                                    geometryStream.getAnimationBuffer());
}

void RenderContext::renderNextFrame() {
  frameRenderer->renderNextFrame();
}

}
