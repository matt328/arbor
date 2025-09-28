#include "core/CoreContext.hpp"

#include "bk/Logger.hpp"
#include "bk/IEventQueue.hpp"

#include "core/AllocatorService.hpp"
#include "core/command-buffers/CommandBufferManager.hpp"
#include "core/pipeline/PipelineManager.hpp"
#include "graphics/common/GraphicsOptions.hpp"

#include "core/Swapchain.hpp"
#include "core/Device.hpp"

#include "PhysicalDevice.hpp"
#include "Surface.hpp"
#include "Instance.hpp"

namespace arb {

CoreContext::CoreContext(std::shared_ptr<bk::IEventQueue> newEventQueue,
                         const GraphicsOptions& newOptions,
                         bk::NativeWindowHandle newWindowHandle)
    : options{newOptions}, eventQueue{std::move(newEventQueue)} {
  Log::trace("Creating CoreContext");

  vulkanInstance = std::make_unique<Instance>(newOptions);
  surface = std::make_unique<Surface>(*vulkanInstance, newWindowHandle.get<HWND>());

  auto allPhysicalDevices = vulkanInstance->enumeratePhysicalDevices(*surface);
  for (const auto& candidate : allPhysicalDevices) {
    if (candidate.isSuitable(*surface)) {
      physicalDevice = std::make_unique<PhysicalDevice>(candidate);
      break;
    }
  }

  device = physicalDevice->createDevice(*surface);

  commandBufferManager = std::make_unique<CommandBufferManager>(*device);

  transferSemaphore = std::make_unique<Semaphore>(*device, true, "TransferQueueSemaphore");
  graphicsSemaphore = std::make_unique<Semaphore>(*device, true, "GraphicsQueueSemaphore");
  computeSemaphore = std::make_unique<Semaphore>(*device, true, "ComputeQueueSemaphore");

  swapchain = std::make_unique<Swapchain>(physicalDevice.get(),
                                          surface.get(),
                                          *device,
                                          eventQueue,
                                          options.initialSize);
  allocatorService =
      std::make_unique<AllocatorService>(physicalDevice->handle(), *device, *vulkanInstance);

  pipelineManager = std::make_unique<PipelineManager>(*device);
}

CoreContext::~CoreContext() {
  Log::trace("Destroying CoreContext");
}

}
