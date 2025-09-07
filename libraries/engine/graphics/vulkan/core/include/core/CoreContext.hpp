#pragma once

#include "bk/IEventQueue.hpp"
#include "bk/NativeWindowHandle.hpp"
#include "Instance.hpp"
#include "PhysicalDevice.hpp"
#include "Surface.hpp"
#include "graphics/common/GraphicsOptions.hpp"

namespace arb {

class Device;
class Swapchain;

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

private:
  GraphicsOptions options;
  Instance vulkanInstance;
  Surface surface;
  PhysicalDevice physicalDevice;
  std::shared_ptr<Device> device;
  std::unique_ptr<Swapchain> swapchain;

  std::shared_ptr<bk::IEventQueue> eventQueue;
};

}
