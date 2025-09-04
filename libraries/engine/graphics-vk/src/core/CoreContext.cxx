#include "CoreContext.hpp"
#include "IEventQueue.hpp"
#include "TestEvents.hpp"
#include "Instance.hpp"
#include "GraphicsOptions.hpp"

namespace arb {

auto makeGraphicsContext(std::shared_ptr<IEventQueue> newEventQueue,
                         const GraphicsOptions& newOptions,
                         bk::NativeWindowHandle newWindowHandle)
    -> std::unique_ptr<IGraphicsContext> {
  return std::make_unique<CoreContext>(std::move(newEventQueue), newOptions, newWindowHandle);
}

CoreContext::CoreContext(std::shared_ptr<IEventQueue> newEventQueue,
                         const GraphicsOptions& newOptions,
                         bk::NativeWindowHandle newWindowHandle)
    : options{newOptions},
      eventQueue{std::move(newEventQueue)},
      vulkanInstance{std::make_shared<Instance>(newOptions)},
      surface{vulkanInstance, newWindowHandle.get<HWND>()} {
  Log->trace("Creating CoreContext");

  auto allPhysicalDevices = vulkanInstance->enumeratePhysicalDevices(surface);
  for (const auto& candidate : allPhysicalDevices) {
    if (candidate.isSuitable(surface)) {
      physicalDevice = candidate;
      break;
    }
  }

  device = physicalDevice.createDevice(surface);

  eventQueue->subscribe<TestEvent>(
      [](std::shared_ptr<TestEvent> event) { Log->trace("TestEvent recieved"); });
}

CoreContext::~CoreContext() {
  Log->trace("Destroying CoreContext");
}

auto CoreContext::run(std::stop_token token) -> void {
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

}
