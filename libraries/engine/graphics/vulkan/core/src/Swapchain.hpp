#pragma once

#include "graphics/common/GraphicsOptions.hpp"

namespace arb {

class PhysicalDevice;
class Surface;
class Device;

class Swapchain : public NonCopyableMovable {
public:
  Swapchain(PhysicalDevice* newPhysicalDevice,
            Surface* newSurface,
            std::shared_ptr<Device> newDevice,
            GraphicsOptions::Size initialSize);
  ~Swapchain();

private:
  PhysicalDevice* physicalDevice;
  Surface* surface;
  std::shared_ptr<Device> device;
  VkExtent2D windowSize{};

  VkSwapchainKHR currentSwapchain{nullptr};
  VkSwapchainKHR oldSwapchain{nullptr};

  auto createSwapchain() -> void;

  static auto chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
      -> VkSurfaceFormatKHR;
  static auto choosePresentMode(const std::vector<VkPresentModeKHR>& presentModes)
      -> VkPresentModeKHR;
  static auto chooseSwapExtent(VkSurfaceCapabilitiesKHR capabilities, VkExtent2D windowSize)
      -> VkExtent2D;
};

}
