#pragma once

#include <vulkan/vulkan_core.h>

#include "bk/NonCopyMove.hpp"
#include "core/QueueFamilyIndices.hpp"

namespace arb {

class PhysicalDevice;

class Device : public NonCopyableMovable {
public:
  Device(VkDevice newDevice,
         PhysicalDevice* newPhysicalDevice,
         QueueFamilyIndices newQueueFamilyIndices);
  ~Device();

  auto waitIdle() -> void;

  [[nodiscard]] auto getGraphicsQueueFamily() const -> uint32_t;
  [[nodiscard]] auto getTransferQueueFamily() const -> uint32_t;
  [[nodiscard]] auto getComputeQueueFamily() const -> uint32_t;
  [[nodiscard]] auto getPresentQueueFamily() const -> uint32_t;

  operator VkDevice() const {
    return vkDevice;
  }

private:
  VkDevice vkDevice;
  const PhysicalDevice* physicalDevice;

  const QueueFamilyIndices queueFamilyIndices{};
};

}
