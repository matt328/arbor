#include "vulkan/base/VulkanContext.hpp"

#include <thread>
#include "Tracy.hpp"
#include <memory>

#include "bk/Logger.hpp"
#include "bk/IEventQueue.hpp"
#include "core/CoreContext.hpp"
#include "engine/common/SimState.hpp"
#include "renderer/RenderContext.hpp"
#include "resources/ResourceContext.hpp"
#include "common/SemaphorePack.hpp"
#include "assets/AssetContext.hpp"

namespace arb {

VulkanContext::VulkanContext(std::shared_ptr<bk::IEventQueue> newEventQueue,
                             IStateBuffer<SimState>& simStateBuffer,
                             const GraphicsOptions& newOptions,
                             bk::NativeWindowHandle newWindowHandle)
    : eventQueue{std::move(newEventQueue)} {
  Log::trace("Creating VulkanContext");

  // TODO: Consider promoting to a SharedContext if too many of these start to accumulate here
  geometryHandleMapper = std::make_unique<GeometryHandleMapper>();

  coreContext = std::make_shared<CoreContext>(eventQueue, newOptions, newWindowHandle);

  resourceContext = std::make_unique<ResourceContext>(
      coreContext->getDevice(),
      coreContext->getAllocatorService(),
      SemaphorePack{.graphics = coreContext->getGraphicsSemaphore(),
                    .transfer = coreContext->getTransferSemaphore(),
                    .compute = coreContext->getComputeSemaphore()});

  renderContext = std::make_unique<RenderContext>(newOptions,
                                                  coreContext->getDevice(),
                                                  coreContext->getSwapchain(),
                                                  simStateBuffer,
                                                  *geometryHandleMapper,
                                                  coreContext->getCommandBufferManager(),
                                                  resourceContext->getResourceFacade(),
                                                  coreContext->getPipelineManager());

  assetContext = std::make_unique<AssetContext>();
}

VulkanContext::~VulkanContext() {
  Log::trace("Destroying Vulkan Context");
}

void VulkanContext::run(std::stop_token token) {
  Log::trace("GraphicsContext::run()");
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
                         const GraphicsOptions& newOptions,
                         bk::NativeWindowHandle newWindowHandle)
    -> std::unique_ptr<IGraphicsContext> {
  return std::make_unique<VulkanContext>(std::move(newEventQueue),
                                         newSimStateBuffer,
                                         newOptions,
                                         newWindowHandle);
}
}
