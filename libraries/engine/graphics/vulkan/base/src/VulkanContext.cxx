#include "vulkan/base/VulkanContext.hpp"

#include <stop_token>
#include "Tracy.hpp"
#include <memory>

#include "bk/Log.hpp"
#include "bk/IEventQueue.hpp"
#include "common/HandleMapperTypes.hpp"
#include "core/CoreContext.hpp"
#include "engine/common/SimState.hpp"
#include "renderer/RenderContext.hpp"
#include "assets/AssetContext.hpp"

namespace arb {

VulkanContext::VulkanContext(std::shared_ptr<bk::IEventQueue> newEventQueue,
                             IStateBuffer<SimState>& simStateBuffer,
                             const EngineOptions& newOptions,
                             bk::NativeWindowHandle newWindowHandle)
    : eventQueue{std::move(newEventQueue)} {
  LOG_TRACE_L1(Log::Core, "Creating VulkanContext");

  // TODO: Consider promoting to a SharedContext if too many of these start to accumulate here
  geometryHandleMapper = std::make_unique<GeometryHandleMapper>();
  textureHandleMapper = std::make_unique<TextureHandleMapper>();

  coreContext = std::make_shared<CoreContext>(eventQueue, newOptions, newWindowHandle);

  renderContext = std::make_unique<RenderContext>(
      newOptions,
      RenderContextDeps{.device = coreContext->getDevice(),
                        .swapchain = coreContext->getSwapchain(),
                        .simStateBuffer = simStateBuffer,
                        .geometryHandleMapper = *geometryHandleMapper,
                        .textureHandleMapper = *textureHandleMapper,
                        .commandBufferManager = coreContext->getCommandBufferManager(),
                        .pipelineManager = coreContext->getPipelineManager(),
                        .eventQueue = *eventQueue,
                        .allocatorService = coreContext->getAllocatorService()});

  assetContext = std::make_unique<AssetContext>();
}

VulkanContext::~VulkanContext() {
  LOG_TRACE_L1(Log::Core, "Destroying Vulkan Context");
}

void VulkanContext::run(std::stop_token token) {
  LOG_TRACE_L1(Log::Core, "VulkanContext::run()");
  using Clock = std::chrono::steady_clock;
  auto currentTime = Clock::now();

  while (!token.stop_requested()) {
    auto newTime = Clock::now();
    auto frameTime = newTime - currentTime;
    if (frameTime > std::chrono::milliseconds(MaxFrameTime)) {
      frameTime = std::chrono::milliseconds(MaxFrameTime);
    }
    currentTime = newTime;
    eventQueue->dispatchPending();
    renderContext->renderNextFrame();
    FrameMark;
  }
}

auto makeGraphicsContext(std::shared_ptr<bk::IEventQueue> newEventQueue,
                         IStateBuffer<SimState>& newSimStateBuffer,
                         const EngineOptions& newOptions,
                         bk::NativeWindowHandle newWindowHandle)
    -> std::unique_ptr<IGraphicsContext> {
  return std::make_unique<VulkanContext>(std::move(newEventQueue),
                                         newSimStateBuffer,
                                         newOptions,
                                         newWindowHandle);
}
}
