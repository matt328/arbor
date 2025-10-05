#include "core/Swapchain.hpp"

#include <algorithm>

#include "core/Device.hpp"
#include "core/ImageView.hpp"
#include "common/ErrorUtils.hpp"
#include "PhysicalDevice.hpp"
#include "Surface.hpp"
#include "common/Semaphore.hpp"
#include "bk/IEventQueue.hpp"

#include "bk/Log.hpp"
#include "engine/common/EngineEvents.hpp"
#include "vulkan/vulkan_core.h"
#include <vulkan/vulkan.h>

namespace arb {

Swapchain::Swapchain(PhysicalDevice* newPhysicalDevice,
                     Surface* newSurface,
                     Device& newDevice,
                     std::shared_ptr<bk::IEventQueue> newEventQueue,
                     VkExtent2D initialSize)
    : physicalDevice{newPhysicalDevice},
      surface{newSurface},
      device{newDevice},
      eventQueue{std::move(newEventQueue)},
      windowSize{initialSize},
      swapchainExtent{},
      swapchainImageFormat{} {
  LOG_TRACE_L1(Log::Core, "Creating Swapchain size: {}x{}", windowSize.width, windowSize.height);
  createSwapchain();
}

Swapchain::~Swapchain() {
  LOG_TRACE_L1(Log::Core, "Destroying Swapchain");
  if (currentSwapchain != VK_NULL_HANDLE) {
    LOG_TRACE_L1(Log::Core, "Destroying CurrentSwapchain");
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
  oldSwapchain = currentSwapchain;
  createSwapchain();
}

auto Swapchain::getImageSemaphore(uint32_t index) -> const Semaphore& {
  return imageSemaphores.at(index);
}

auto Swapchain::getImageCount() const -> uint32_t {
  return swapchainImages.size();
}

auto Swapchain::getImage(uint32_t index) const -> Image& {
  return *swapchainImages.at(index);
}

auto Swapchain::getImageView(uint32_t index) const -> ImageView& {
  return *swapchainImageViews.at(index);
}

auto Swapchain::getImageHandle(uint32_t index) const -> ImageHandle {
  return swapchainImageHandles.at(index);
}

auto Swapchain::getExtent() const -> VkExtent2D {
  return swapchainExtent;
}

auto Swapchain::getFormat() const -> VkFormat {
  return swapchainImageFormat;
}

auto Swapchain::createSwapchain() -> void {
  vkDeviceWaitIdle(device);
  if (oldSwapchain != VK_NULL_HANDLE) {
    // Clear out any images/views/semaphores
    LOG_TRACE_L1(Log::Core, "Recreating swapchain");
    swapchainImages.clear();
    swapchainImageViews.clear();
    swapchainImageHandles.clear();
    imageSemaphores.clear();
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

  vkDestroySwapchainKHR(device, oldSwapchain, nullptr);

  uint32_t currentImageCount{};
  checkVk(vkGetSwapchainImagesKHR(device, currentSwapchain, &currentImageCount, nullptr),
          "vkGetSwapchainImagesKHR(count)");

  auto swapchainTempImages = std::vector<VkImage>{};
  swapchainTempImages.resize(currentImageCount);
  checkVk(vkGetSwapchainImagesKHR(device,
                                  currentSwapchain,
                                  &currentImageCount,
                                  swapchainTempImages.data()),
          "vkGetSwapchainImagesKHR(data)");
  {
    auto index = 0;
    for (const auto& img : swapchainTempImages) {
      swapchainImages.emplace_back(
          std::make_unique<Image>(&device,
                                  img,
                                  swapchainExtent,
                                  swapchainImageFormat,
                                  std::format("SwapchainImage-{}", index)));
      swapchainImageHandles.push_back(imageHandleGenerator.requestHandle());

      const auto createInfo = VkImageViewCreateInfo{
          .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
          .image = img,
          .viewType = VK_IMAGE_VIEW_TYPE_2D,
          .format = swapchainImageFormat,
          .components =
              VkComponentMapping{
                  .r = VK_COMPONENT_SWIZZLE_R,
                  .g = VK_COMPONENT_SWIZZLE_G,
                  .b = VK_COMPONENT_SWIZZLE_B,
                  .a = VK_COMPONENT_SWIZZLE_A,
              },
          .subresourceRange = VkImageSubresourceRange{.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                                      .baseMipLevel = 0,
                                                      .levelCount = 1,
                                                      .baseArrayLayer = 0,
                                                      .layerCount = 1}};
      swapchainImageViews.push_back(
          std::make_unique<ImageView>(device,
                                      createInfo,
                                      swapchainImageHandles.back(),
                                      std::format("SwapchainImageView-{}", index)));
      ++index;
    }
  }

  size_t index{};
  for (const auto& image : swapchainImages) {
    std::optional<std::string> semaphoreName{};
#ifdef DEBUG
    semaphoreName.emplace(std::format("Swapchain Image Semaphore {}", index));
#endif
    imageSemaphores.emplace_back(device, false, semaphoreName);
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
