#include "GraphicsContext.hpp"
#include "IEventQueue.hpp"
#include "TestEvents.hpp"
#include "vk/Instance.hpp"
#include "GraphicsOptions.hpp"

namespace arb {

auto makeGraphicsContext(std::shared_ptr<IEventQueue> newEventQueue,
                         const GraphicsOptions& newOptions,
                         bk::NativeWindowHandle newWindowHandle)
    -> std::unique_ptr<IGraphicsContext> {
  return std::make_unique<GraphicsContext>(std::move(newEventQueue), newOptions, newWindowHandle);
}

GraphicsContext::GraphicsContext(std::shared_ptr<IEventQueue> newEventQueue,
                                 const GraphicsOptions& newOptions,
                                 bk::NativeWindowHandle newWindowHandle)
    : options{newOptions},
      eventQueue{std::move(newEventQueue)},
      vulkanInstance{std::make_shared<Instance>(newOptions)},
      surface{vulkanInstance, newWindowHandle.get<HWND>()} {
  Log->trace("Creating GraphicsContext");

  auto physicalDevices = vulkanInstance->enumeratePhysicalDevices();
  Log->trace("Found {} physical devices", physicalDevices.size());

  physicalDevice = PhysicalDevice{selectPhysicalDevice(physicalDevices)};

  device = physicalDevice.createDevice(surface);

  eventQueue->subscribe<TestEvent>(
      [](std::shared_ptr<TestEvent> event) { Log->trace("TestEvent recieved"); });
}

GraphicsContext::~GraphicsContext() {
  Log->trace("Destroying GraphicsContext");
}

auto GraphicsContext::run(std::stop_token token) -> void {
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

auto GraphicsContext::selectPhysicalDevice(const std::vector<VkPhysicalDevice>& physicalDevices)
    -> VkPhysicalDevice {
  for (const auto& device : physicalDevices) {
    if (DeviceOptions::supports(device, surface)) {
      return device;
    }
  }
  throw std::runtime_error("Failed to find a Physical Device");
}

}
