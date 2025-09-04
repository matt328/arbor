#pragma once

#include "IGraphicsContext.hpp"
#include "bk/NativeWindowHandle.hpp"
#include "Instance.hpp"
#include "PhysicalDevice.hpp"
#include "Surface.hpp"

namespace arb {

class Device;

class CoreContext : public IGraphicsContext {
public:
  CoreContext(std::shared_ptr<IEventQueue> newEventQueue,
              const GraphicsOptions& newOptions,
              bk::NativeWindowHandle newWindowHandle);
  ~CoreContext() override;

  auto run(std::stop_token token) -> void override;

private:
  GraphicsOptions options;
  std::shared_ptr<Instance> vulkanInstance;
  Surface surface;
  PhysicalDevice physicalDevice;
  std::shared_ptr<Device> device;

  std::shared_ptr<IEventQueue> eventQueue;
};

}
