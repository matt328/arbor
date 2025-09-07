#include "vulkan/base/VulkanContext.hpp"
#include "bk/IEventQueue.hpp"
#include "core/CoreContext.hpp"
#include "engine/common/SimState.hpp"
#include "renderer/RenderContext.hpp"

namespace arb {

VulkanContext::VulkanContext(std::shared_ptr<bk::IEventQueue> newEventQueue,
                             IStateBuffer<SimState>& simStateBuffer,
                             const GraphicsOptions& newOptions,
                             bk::NativeWindowHandle newWindowHandle)
    : eventQueue{std::move(newEventQueue)} {
  Log->trace("Creating VulkanContext");
  coreContext = std::make_shared<CoreContext>(eventQueue, newOptions, newWindowHandle);
  renderContext = std::make_unique<RenderContext>(newOptions,
                                                  coreContext->getDevice(),
                                                  coreContext->getSwapchain(),
                                                  simStateBuffer);
}

VulkanContext::~VulkanContext() {
  Log->trace("Destroying Vulkan Context");
}

auto VulkanContext::run(std::stop_token token) -> void {
  Log->trace("GraphicsContext::run()");
  using clock = std::chrono::steady_clock;
  constexpr int targetHz = 1;
  constexpr auto timestep = std::chrono::nanoseconds(1'000'000'000 / targetHz);

  auto nextTick = clock::now();
  while (!token.stop_requested()) {
    auto now = clock::now();
    if (now >= nextTick) {
      Log->trace("graphicsContext tick()");
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
