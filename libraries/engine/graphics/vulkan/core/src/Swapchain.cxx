#include "Swapchain.hpp"
#include "Device.hpp"
#include "PhysicalDevice.hpp"
#include "Surface.hpp"

namespace arb {

Swapchain::Swapchain(PhysicalDevice* newPhysicalDevice,
                     Surface* newSurface,
                     std::shared_ptr<Device> newDevice,
                     GraphicsOptions::Size initialSize)
    : physicalDevice{newPhysicalDevice},
      surface{newSurface},
      device{std::move(newDevice)},
      windowSize{initialSize.width, initialSize.height} {
  Log->trace("Creating Swapchain size: {}x{}", windowSize.width, windowSize.height);
  createSwapchain();
}

Swapchain::~Swapchain() {
  Log->trace("Destroying Swapchain");
}

auto Swapchain::createSwapchain() -> void {
  device->waitIdle();
  if (oldSwapchain != nullptr) {
    // Clear out any images/views/semaphores
  }

  const auto queueFamilyInfo = physicalDevice->findQueueFamilies(surface);

  const auto [capabilities, formats, presentModes] = physicalDevice->querySwapchainSupport(surface);

  const auto surfaceFormat = chooseSurfaceFormat(formats);
  const auto presentMode = choosePresentMode(presentModes);
  const auto extent = chooseSwapExtent(capabilities, windowSize);
}

auto Swapchain::chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
    -> VkSurfaceFormatKHR {
  // Prefer SRGB Nonlinear, otherwise, whatever just grab the first one I guess.
  for (const auto& availableFormat : formats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
        availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return availableFormat;
    }
  }
  return formats[0];
}

auto Swapchain::choosePresentMode(const std::vector<VkPresentModeKHR>& presentModes)
    -> VkPresentModeKHR {
  // Choose any present mode you like, as long as it's FIFO
  for (const auto& availablePresentMode : presentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_FIFO_KHR) {
      return availablePresentMode;
    }
  }
  return VK_PRESENT_MODE_FIFO_KHR;
}

auto Swapchain::chooseSwapExtent(VkSurfaceCapabilitiesKHR capabilities, VkExtent2D windowSize)
    -> VkExtent2D {
  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  }
  const auto& [width, height] = windowSize;
  VkExtent2D actualExtent = {.width = width, .height = height};

  actualExtent.width = std::clamp(actualExtent.width,
                                  capabilities.minImageExtent.width,
                                  capabilities.maxImageExtent.width);
  actualExtent.height = std::clamp(actualExtent.height,
                                   capabilities.minImageExtent.height,
                                   capabilities.maxImageExtent.height);

  return actualExtent;
}

}
