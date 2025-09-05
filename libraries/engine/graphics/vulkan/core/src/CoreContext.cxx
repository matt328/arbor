#include "core/CoreContext.hpp"
#include "bk/IEventQueue.hpp"
#include "Instance.hpp"
#include "Swapchain.hpp"
#include "graphics/common/GraphicsOptions.hpp"

namespace arb {

CoreContext::CoreContext(std::shared_ptr<bk::IEventQueue> newEventQueue,
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

  swapchain = std::make_shared<Swapchain>(&physicalDevice, &surface, device, options.initialSize);
}

CoreContext::~CoreContext() {
  Log->trace("Destroying CoreContext");
}

}
