#pragma once

namespace arb {

class PhysicalDevice;

class Device : public NonCopyableMovable {
public:
  Device(VkDevice newDevice, PhysicalDevice* newPhysicalDevice);
  ~Device();

private:
  VkDevice vkDevice;
  const PhysicalDevice* physicalDevice;
};

}
