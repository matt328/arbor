#pragma once

#include "bk/IEventQueue.hpp"
#include "bk/NativeWindowHandle.hpp"
#include "bk/NonCopyMove.hpp"

#include "graphics/common/GraphicsOptions.hpp"

namespace arb {

class Device;
class Swapchain;
class Instance;
class Surface;
class PhysicalDevice;
class AllocatorService;

class CoreContext : NonCopyableMovable {
public:
  CoreContext(std::shared_ptr<bk::IEventQueue> newEventQueue,
              const GraphicsOptions& newOptions,
              bk::NativeWindowHandle newWindowHandle);
  ~CoreContext();

  [[nodiscard]] auto getDevice() const -> Device& {
    return *device;
  }

  [[nodiscard]] auto getSwapchain() const -> Swapchain& {
    return *swapchain;
  }

  [[nodiscard]] auto getAllocatorService() const -> AllocatorService& {
    return *allocatorService;
  }

private:
  GraphicsOptions options;
  std::unique_ptr<Instance> vulkanInstance;
  std::unique_ptr<Surface> surface;
  std::unique_ptr<PhysicalDevice> physicalDevice;
  std::shared_ptr<Device> device;
  std::unique_ptr<Swapchain> swapchain;
  std::unique_ptr<AllocatorService> allocatorService;

  std::shared_ptr<bk::IEventQueue> eventQueue;
};

}
