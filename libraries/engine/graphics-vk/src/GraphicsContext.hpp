#pragma once

#include "IGraphicsContext.hpp"
#include "bk/NativeWindowHandle.hpp"
#include "vk/Instance.hpp"
#include "vk/PhysicalDevice.hpp"
#include "vk/Surface.hpp"

namespace arb {

class Device;

class GraphicsContext : public IGraphicsContext {
public:
  GraphicsContext(std::shared_ptr<IEventQueue> newEventQueue,
                  const GraphicsOptions& newOptions,
                  bk::NativeWindowHandle newWindowHandle);
  virtual ~GraphicsContext() override;

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
