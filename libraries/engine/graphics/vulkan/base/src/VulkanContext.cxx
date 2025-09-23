#include "vulkan/base/VulkanContext.hpp"

#include <thread>
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
                                                  resourceContext->getResourceFacade());

  assetContext = std::make_unique<AssetContext>();
}

VulkanContext::~VulkanContext() {
  Log::trace("Destroying Vulkan Context");
}

auto VulkanContext::run(std::stop_token token) -> void {
  Log::trace("GraphicsContext::run()");
  using clock = std::chrono::steady_clock;
  constexpr int targetHz = 1;
  constexpr auto timestep = std::chrono::nanoseconds(1'000'000'000 / targetHz);

  auto nextTick = clock::now();
  while (!token.stop_requested()) {
    auto now = clock::now();
    if (now >= nextTick) {
      Log::trace("graphicsContext tick()");
      renderContext->renderNextFrame();
      eventQueue->dispatchPending();
      nextTick += timestep;
      if (now > nextTick + timestep * 10) {
        nextTick = now;
      }
    } else {
      std::this_thread::sleep_until(nextTick);
    }
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
