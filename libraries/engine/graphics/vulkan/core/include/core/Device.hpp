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

  auto graphicsQueue() {
    return graphics;
  }

  auto presentQueue() {
    return present;
  }

  auto computeQueue() {
    return compute;
  }

  auto transferQueue() {
    return transfer;
  }

private:
  VkDevice vkDevice;
  const PhysicalDevice* physicalDevice;

  const QueueFamilyIndices queueFamilyIndices{};
  VkQueue graphics{VK_NULL_HANDLE};
  VkQueue present{VK_NULL_HANDLE};
  VkQueue compute{VK_NULL_HANDLE};
  VkQueue transfer{VK_NULL_HANDLE};
};

}
