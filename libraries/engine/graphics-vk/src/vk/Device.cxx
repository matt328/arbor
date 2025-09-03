#include "Device.hpp"

namespace arb {

Device::Device(VkDevice newDevice, PhysicalDevice* newPhysicalDevice)
    : vkDevice{newDevice}, physicalDevice{newPhysicalDevice} {
  Log->trace("Creating Device");
}

Device::~Device() {
  if (vkDevice != nullptr) {
    vkDestroyDevice(vkDevice, nullptr);
    Log->trace("Destroyed Device");
  }
}

}
