#include <utility>

#include "core/Device.hpp"

#include "PhysicalDevice.hpp"
#include "bk/Logger.hpp"

namespace arb {

Device::Device(VkDevice newDevice,
               PhysicalDevice* newPhysicalDevice,
               QueueFamilyIndices newQueueFamilyIndices)
    : vkDevice{newDevice},
      physicalDevice{newPhysicalDevice},
      queueFamilyIndices{std::move(newQueueFamilyIndices)} {
  Log::trace("Creating Device");
}

Device::~Device() {
  if (vkDevice != nullptr) {
    vkDestroyDevice(vkDevice, nullptr);
    Log::trace("Destroyed Device");
  }
}

[[nodiscard]] auto Device::getGraphicsQueueFamily() const -> uint32_t {
  return *queueFamilyIndices.graphicsFamily;
}

[[nodiscard]] auto Device::getTransferQueueFamily() const -> uint32_t {
  return *queueFamilyIndices.transferFamily;
}

[[nodiscard]] auto Device::getComputeQueueFamily() const -> uint32_t {
  return *queueFamilyIndices.computeFamily;
}

[[nodiscard]] auto Device::getPresentQueueFamily() const -> uint32_t {
  return *queueFamilyIndices.presentFamily;
}

auto Device::waitIdle() -> void {
  vkDeviceWaitIdle(vkDevice);
}

}
