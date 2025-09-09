#pragma once

#include <vulkan/vulkan_core.h>

#include "bk/NonCopyMove.hpp"

namespace arb {

class PhysicalDevice;

class Device : public NonCopyableMovable {
public:
  Device(VkDevice newDevice, PhysicalDevice* newPhysicalDevice);
  ~Device();

  auto waitIdle() -> void;

  operator VkDevice() const {
    return vkDevice;
  }

private:
  VkDevice vkDevice;
  const PhysicalDevice* physicalDevice;
};

}
