#include "core/Swapchain.hpp"
#include "core/Device.hpp"
#include "ErrorUtils.hpp"
#include "PhysicalDevice.hpp"
#include "Surface.hpp"
#include "common/ImageView.hpp"
#include "common/Semaphore.hpp"
#include "bk/IEventQueue.hpp"
#include "bk/Logger.hpp"
#include "engine/common/EngineEvents.hpp"

namespace arb {

Swapchain::Swapchain(PhysicalDevice* newPhysicalDevice,
                     Surface* newSurface,
                     VkDevice newDevice,
                     std::shared_ptr<bk::IEventQueue> newEventQueue,
                     GraphicsOptions::Size initialSize)
    : physicalDevice{newPhysicalDevice},
      surface{newSurface},
      device{newDevice},
      eventQueue{std::move(newEventQueue)},
      windowSize{initialSize.width, initialSize.height} {
  Log::trace("Creating Swapchain size: {}x{}", windowSize.width, windowSize.height);
  createSwapchain();
}

Swapchain::~Swapchain() {
  Log::trace("Destroying Swapchain");
  if (currentSwapchain != VK_NULL_HANDLE) {
    vkDestroySwapchainKHR(device, currentSwapchain, nullptr);
  }
}

auto Swapchain::acquireNextImage(const Semaphore& semaphore)
    -> std::variant<uint32_t, ImageAcquireResult> {
  uint32_t imageIndex{};
  auto result = vkAcquireNextImageKHR(device,
                                      currentSwapchain,
                                      UINT64_MAX,
                                      semaphore,
                                      VK_NULL_HANDLE,
                                      &imageIndex);
  if (result == VK_SUCCESS) {
    return imageIndex;
  }

  if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR) {
    return ImageAcquireResult::NeedsResize;
  }

  return ImageAcquireResult::Error;
}

auto Swapchain::recreate() -> void {
}

auto Swapchain::createSwapchain() -> void {
  vkDeviceWaitIdle(device);
  if (oldSwapchain != nullptr) {
    // Clear out any images/views/semaphores
    swapchainImages.clear();
    swapchainImageViews.clear();
  }

  const auto queueFamilyInfo = physicalDevice->findQueueFamilies(surface);

  const auto [capabilities, formats, presentModes] = physicalDevice->querySwapchainSupport(surface);

  const auto surfaceFormat = chooseSurfaceFormat(formats);
  swapchainImageFormat = surfaceFormat.format;
  const auto presentMode = choosePresentMode(presentModes);
  swapchainExtent = chooseSwapExtent(capabilities, windowSize);

  // One over the min, but not if it exceeds the max
  const auto imageCount =
      std::min(capabilities.minImageCount + 1,
               capabilities.maxImageCount > 0 ? capabilities.maxImageCount : UINT32_MAX);

  auto swapchainCreateInfo = VkSwapchainCreateInfoKHR{
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .surface = *surface,
      .minImageCount = imageCount,
      .imageFormat = swapchainImageFormat,
      .imageColorSpace = surfaceFormat.colorSpace,
      .imageExtent = swapchainExtent,
      .imageArrayLayers = 1,
      .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
      .preTransform = capabilities.currentTransform,
      .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      .presentMode = presentMode,
      .clipped = VK_TRUE,
      .oldSwapchain = VK_NULL_HANDLE};

  if (oldSwapchain != VK_NULL_HANDLE) {
    swapchainCreateInfo.oldSwapchain = oldSwapchain;
  }

  const auto queueFamilyIndices =
      std::array{queueFamilyInfo.graphicsFamily.value(), queueFamilyInfo.presentFamily.value()};

  if (queueFamilyInfo.graphicsFamily != queueFamilyInfo.presentFamily) {
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    swapchainCreateInfo.queueFamilyIndexCount = 2;
    swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices.data();
  } else {
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.queueFamilyIndexCount = 1;
    swapchainCreateInfo.pQueueFamilyIndices = &queueFamilyInfo.graphicsFamily.value();
  }

  checkVk(vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &currentSwapchain),
          "vkCreateSwapchainKHR");

  uint32_t currentImageCount{};
  checkVk(vkGetSwapchainImagesKHR(device, currentSwapchain, &currentImageCount, nullptr),
          "vkGetSwapchainImagesKHR(count)");
  swapchainImages.resize(currentImageCount);
  checkVk(
      vkGetSwapchainImagesKHR(device, currentSwapchain, &currentImageCount, swapchainImages.data()),
      "vkGetSwapchainImagesKHR(data)");

  constexpr VkComponentMapping components{
      .r = VK_COMPONENT_SWIZZLE_R,
      .g = VK_COMPONENT_SWIZZLE_G,
      .b = VK_COMPONENT_SWIZZLE_B,
      .a = VK_COMPONENT_SWIZZLE_A,
  };

  constexpr VkImageSubresourceRange subersourceRange{.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                                     .baseMipLevel = 0,
                                                     .levelCount = 1,
                                                     .baseArrayLayer = 0,
                                                     .layerCount = 1};
  size_t index{};
  for (const auto& image : swapchainImages) {
    const auto createInfo = VkImageViewCreateInfo{.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                                                  .image = image,
                                                  .viewType = VK_IMAGE_VIEW_TYPE_2D,
                                                  .format = swapchainImageFormat,
                                                  .components = components,
                                                  .subresourceRange = subersourceRange};
    std::optional<std::string> imageViewName{};
    std::optional<std::string> semaphoreName{};
#ifdef DEBUG
    imageViewName.emplace(std::format("Swapchain ImageView {}", index));
    semaphoreName.emplace(std::format("Swapchain Image Semaphore {}", index));
#endif
    swapchainImageViews.emplace_back(device, createInfo, imageViewName);
    imageSemaphores.emplace_back(device, semaphoreName);
    ++index;
  }

  if (oldSwapchain != VK_NULL_HANDLE) {
    eventQueue->emitEvent(
        RenderAreaResized{.width = swapchainExtent.width, .height = swapchainExtent.height});
  } else {
    eventQueue->emitEvent(
        RenderAreaCreated{.width = swapchainExtent.width, .height = swapchainExtent.height});
  }
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
