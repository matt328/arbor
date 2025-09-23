#pragma once

#include <memory>
#include <variant>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "graphics/common/GraphicsOptions.hpp"
#include "common/ImageAcquireResult.hpp"
#include "bk/NonCopyMove.hpp"

namespace bk {
class IEventQueue;
}

namespace arb {

class PhysicalDevice;
class Surface;
class Device;
class ImageView;
class Semaphore;

class Swapchain : public NonCopyableMovable {
public:
  Swapchain(PhysicalDevice* newPhysicalDevice,
            Surface* newSurface,
            VkDevice newDevice,
            std::shared_ptr<bk::IEventQueue> newEventQueue,
            GraphicsOptions::Size initialSize);
  ~Swapchain();

  auto acquireNextImage(const Semaphore& semaphore) -> std::variant<uint32_t, ImageAcquireResult>;

  auto recreate() -> void;

private:
  PhysicalDevice* physicalDevice;
  Surface* surface;
  VkDevice device;
  std::shared_ptr<bk::IEventQueue> eventQueue;
  VkExtent2D windowSize{};

  VkSwapchainKHR currentSwapchain{nullptr};
  VkSwapchainKHR oldSwapchain{nullptr};

  VkExtent2D swapchainExtent;
  VkFormat swapchainImageFormat;

  std::vector<VkImage> swapchainImages;
  std::vector<Semaphore> imageSemaphores;

  auto createSwapchain() -> void;

  static auto chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
      -> VkSurfaceFormatKHR;
  static auto choosePresentMode(const std::vector<VkPresentModeKHR>& presentModes)
      -> VkPresentModeKHR;
  static auto chooseSwapExtent(VkSurfaceCapabilitiesKHR capabilities, VkExtent2D windowSize)
      -> VkExtent2D;
};

}
