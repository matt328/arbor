#include "PhysicalDevice.hpp"

#include <algorithm>
#include <unordered_set>

#include "bk/Logger.hpp"

#include "core/Device.hpp"
#include "DeviceFeatures.hpp"
#include "ErrorUtils.hpp"
#include "Surface.hpp"

namespace arb {

DeviceFeatures PhysicalDevice::RequiredFeatures = [] {
  DeviceFeatures features;
  features.features2.features.samplerAnisotropy = VK_TRUE;

  features.vk12.drawIndirectCount = VK_TRUE;
  features.vk12.bufferDeviceAddress = VK_TRUE;
  features.vk12.descriptorBindingPartiallyBound = VK_TRUE;
  features.vk12.runtimeDescriptorArray = VK_TRUE;
  features.vk12.descriptorIndexing = VK_TRUE;

  features.drawParams.shaderDrawParameters = VK_TRUE;

  features.dynamicRendering.dynamicRendering = VK_TRUE;
  features.extDyn.extendedDynamicState = VK_TRUE;

  features.sync2.synchronization2 = VK_TRUE;

  return features;
}();

PhysicalDevice::PhysicalDevice(VkPhysicalDevice handle) : vkPhysicalDevice{handle} {
  Log::trace("Constructed PhysicalDevice");
  vkGetPhysicalDeviceProperties(vkPhysicalDevice, &properties);
  Log::info("Using GPU: {} (API version {}.{}.{}), Driver version {}, Vendor ID: {}, Device ID: "
            "{}, Type: {}",
            properties.deviceName,
            VK_VERSION_MAJOR(properties.apiVersion),
            VK_VERSION_MINOR(properties.apiVersion),
            VK_VERSION_PATCH(properties.apiVersion),
            properties.driverVersion,
            properties.vendorID,
            properties.deviceID,
            static_cast<int>(properties.deviceType));
}

PhysicalDevice::~PhysicalDevice() {
  Log::trace("Destroyed PhysicalDevice");
}

auto PhysicalDevice::isSuitable(const Surface& surface) const -> bool {
  return supportsExtensions() && supportsFeatures() && findQueueFamilies(&surface).isComplete();
  return true;
}

auto PhysicalDevice::supportsExtensions() const -> bool {
  uint32_t extCount = 0;
  vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &extCount, nullptr);
  std::vector<VkExtensionProperties> available(extCount);
  vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &extCount, available.data());

  auto properties = VkPhysicalDeviceProperties{};
  vkGetPhysicalDeviceProperties(vkPhysicalDevice, &properties);
  auto* deviceName = properties.deviceName;

  for (const auto* req : RequiredExtensions) {
    auto found = std::ranges::any_of(available, [&](const VkExtensionProperties& ext) {
      return std::strcmp(ext.extensionName, req) == 0;
    });
    if (!found) {
      Log::warn("Device {}, Required Extension {} not present", deviceName, req);
      return false;
    }
  }
  return true;
}

auto PhysicalDevice::supportsFeatures() const -> bool {
  DeviceFeatures supported;
  vkGetPhysicalDeviceFeatures2(vkPhysicalDevice, &supported.features2);
  // Core features
  checkFeature("samplerAnisotropy",
               PhysicalDevice::RequiredFeatures.features2.features,
               supported.features2.features,
               &VkPhysicalDeviceFeatures::samplerAnisotropy);
  // Vulkan 1.2 Features
  checkFeature("drawIndirectCount",
               PhysicalDevice::RequiredFeatures.vk12,
               supported.vk12,
               &VkPhysicalDeviceVulkan12Features::drawIndirectCount);
  checkFeature("bufferDeviceAddress",
               PhysicalDevice::RequiredFeatures.vk12,
               supported.vk12,
               &VkPhysicalDeviceVulkan12Features::bufferDeviceAddress);
  checkFeature("descriptorBindingPartiallyBound",
               PhysicalDevice::RequiredFeatures.vk12,
               supported.vk12,
               &VkPhysicalDeviceVulkan12Features::descriptorBindingPartiallyBound);
  checkFeature("runtimeDescriptorArray",
               PhysicalDevice::RequiredFeatures.vk12,
               supported.vk12,
               &VkPhysicalDeviceVulkan12Features::runtimeDescriptorArray);
  checkFeature("descriptorIndexing",
               PhysicalDevice::RequiredFeatures.vk12,
               supported.vk12,
               &VkPhysicalDeviceVulkan12Features::descriptorIndexing);
  // Shader Draw Parameters
  checkFeature("shaderDrawParameters",
               PhysicalDevice::RequiredFeatures.drawParams,
               supported.drawParams,
               &VkPhysicalDeviceShaderDrawParametersFeatures::shaderDrawParameters);
  // Dynamic Rendering
  checkFeature("dynamicRendering",
               PhysicalDevice::RequiredFeatures.dynamicRendering,
               supported.dynamicRendering,
               &VkPhysicalDeviceDynamicRenderingFeaturesKHR::dynamicRendering);
  // Extended Dynamic State
  checkFeature("extendedDynamicState",
               PhysicalDevice::RequiredFeatures.extDyn,
               supported.extDyn,
               &VkPhysicalDeviceExtendedDynamicStateFeaturesEXT::extendedDynamicState);
  // Synchronization 2
  checkFeature("synchronization2",
               PhysicalDevice::RequiredFeatures.sync2,
               supported.sync2,
               &VkPhysicalDeviceSynchronization2Features::synchronization2);

  return true;
}

auto PhysicalDevice::createDevice(const Surface& surface) -> std::shared_ptr<Device> {

  // Queue Families
  auto queueFamilies = findQueueFamilies(&surface);
  logQueueFamilyIndices(queueFamilies);

  auto queueCreateInfos = std::vector<VkDeviceQueueCreateInfo>{};
  getQueueCreateInfo(queueCreateInfos, queueFamilies);

  VkDeviceCreateInfo deviceCreateInfo{};
  deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceCreateInfo.pNext = &RequiredFeatures.features2;
  deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
  deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

  auto requiredExtensions = RequiredExtensions;
  deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
  deviceCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();

  // 6. Create the logical device
  VkDevice vkDevice = nullptr;
  checkVk(vkCreateDevice(vkPhysicalDevice, &deviceCreateInfo, nullptr, &vkDevice),
          "Failed to create logical device");

  return std::make_shared<Device>(vkDevice, this);
}

auto PhysicalDevice::handle() const -> VkPhysicalDevice {
  return vkPhysicalDevice;
}

[[nodiscard]] auto PhysicalDevice::querySwapchainSupport(const Surface* surface) const
    -> SwapchainSupportDetails {
  auto capabilities = VkSurfaceCapabilitiesKHR{};
  checkVk(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice, *surface, &capabilities),
          "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");

  uint32_t formatCount;
  checkVk(vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, *surface, &formatCount, nullptr),
          "vkGetPhysicalDeviceSurfaceFormatsKHR(count)");
  auto surfaceFormats = std::vector<VkSurfaceFormatKHR>{formatCount};
  checkVk(vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice,
                                               *surface,
                                               &formatCount,
                                               surfaceFormats.data()),
          "vkGetPhysicalDeviceSurfaceFormatsKHR(data)");
  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, *surface, &presentModeCount, nullptr);
  std::vector<VkPresentModeKHR> presentModes(presentModeCount);
  vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice,
                                            *surface,
                                            &presentModeCount,
                                            presentModes.data());
  return {.capabilities = capabilities, .formats = surfaceFormats, .presentModes = presentModes};
}

auto PhysicalDevice::findQueueFamilies(const Surface* surface) const -> QueueFamilyIndices {
  QueueFamilyIndices queueFamilyIndices{};

  uint32_t familyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &familyCount, nullptr);
  auto queueFamilies = std::vector<VkQueueFamilyProperties>(familyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &familyCount, queueFamilies.data());

  std::optional<uint32_t> graphicsFamilyIndex;
  std::optional<uint32_t> computeFamilyIndex;
  std::optional<uint32_t> transferFamilyIndex;

  for (uint32_t i = 0; i < queueFamilies.size(); i++) {
    const auto& queueFamily = queueFamilies[i];
    if ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0u) {
      if (!queueFamilyIndices.graphicsFamily.has_value()) {
        queueFamilyIndices.graphicsFamily = i;
        queueFamilyIndices.graphicsFamilyCount = queueFamily.queueCount;
        queueFamilyIndices.graphicsFamilyPriorities.assign(queueFamily.queueCount, 0.f);
        queueFamilyIndices.graphicsFamilyPriorities[0] = 1.f;
        graphicsFamilyIndex = i;
      }
    }

    VkBool32 supported{};
    checkVk(vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, i, *surface, &supported),
            "vkGetPhysicalDeviceSurfaceSupportKHR");
    if (supported != 0u) {
      if (!queueFamilyIndices.presentFamily.has_value()) {
        queueFamilyIndices.presentFamily = i;
        queueFamilyIndices.presentFamilyCount = queueFamily.queueCount;
        queueFamilyIndices.presentFamilyPriorities.assign(queueFamily.queueCount, 0.f);
        queueFamilyIndices.presentFamilyPriorities[0] = 1.f;
      }
    }

    if (((queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0u) &&
        ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0u)) {
      if (!queueFamilyIndices.computeFamily.has_value()) {
        queueFamilyIndices.computeFamily = i;
        queueFamilyIndices.computeFamilyCount = queueFamily.queueCount;
        queueFamilyIndices.computeFamilyPriorities.assign(queueFamily.queueCount, 0.f);
        queueFamilyIndices.computeFamilyPriorities[0] = 1.f;
        computeFamilyIndex = i;
      }
    }

    if (((queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0u) &&
        ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0u) &&
        ((queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) == 0u)) {
      if (!queueFamilyIndices.transferFamily.has_value()) {
        queueFamilyIndices.transferFamily = i;
        queueFamilyIndices.transferFamilyCount = queueFamily.queueCount;
        queueFamilyIndices.transferFamilyPriorities.assign(queueFamily.queueCount, 0.f);
        queueFamilyIndices.transferFamilyPriorities[0] = 1.f;
        transferFamilyIndex = i;
      }
    }
  }

  // Fallbacks
  if (!queueFamilyIndices.computeFamily.has_value() && graphicsFamilyIndex.has_value()) {
    queueFamilyIndices.computeFamily = graphicsFamilyIndex;
    queueFamilyIndices.computeFamilyCount = queueFamilyIndices.graphicsFamilyCount;
    queueFamilyIndices.computeFamilyPriorities = queueFamilyIndices.graphicsFamilyPriorities;
  }

  if (!queueFamilyIndices.transferFamily.has_value() && graphicsFamilyIndex.has_value()) {
    queueFamilyIndices.transferFamily = graphicsFamilyIndex;
    queueFamilyIndices.transferFamilyCount = queueFamilyIndices.graphicsFamilyCount;
    queueFamilyIndices.transferFamilyPriorities = queueFamilyIndices.graphicsFamilyPriorities;
  }

  return queueFamilyIndices;
}

auto PhysicalDevice::getQueueCreateInfo(std::vector<VkDeviceQueueCreateInfo>& queueCreateInfo,
                                        const QueueFamilyIndices& queueFamilyIndices) -> void {
  std::unordered_set<uint32_t> usedQueueFamilies;

  // Graphics Queue(s)
  if (queueFamilyIndices.graphicsFamily.has_value() &&
      queueFamilyIndices.graphicsFamilyCount.has_value()) {
    const uint32_t index = queueFamilyIndices.graphicsFamily.value();
    usedQueueFamilies.insert(index);

    const auto graphicsFamilyCreateInfo = VkDeviceQueueCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = index,
        .queueCount = queueFamilyIndices.graphicsFamilyCount.value(),
        .pQueuePriorities = queueFamilyIndices.graphicsFamilyPriorities.data()};
    queueCreateInfo.push_back(graphicsFamilyCreateInfo);
  }

  // Present Queue(s) — only if different from graphics
  if (queueFamilyIndices.presentFamily.has_value() &&
      queueFamilyIndices.presentFamilyCount.has_value()) {
    const uint32_t index = queueFamilyIndices.presentFamily.value();
    if (!usedQueueFamilies.contains(index)) {
      Log::trace("Device supports separate present queue");

      usedQueueFamilies.insert(index);
      const auto presentFamilyCreateInfo = VkDeviceQueueCreateInfo{
          .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
          .queueFamilyIndex = index,
          .queueCount = queueFamilyIndices.presentFamilyCount.value(),
          .pQueuePriorities = queueFamilyIndices.presentFamilyPriorities.data()};
      queueCreateInfo.push_back(presentFamilyCreateInfo);
    }
  }

  // Transfer Queue(s) — only if different from already used
  if (queueFamilyIndices.transferFamily.has_value() &&
      queueFamilyIndices.transferFamilyCount.has_value()) {
    const uint32_t index = queueFamilyIndices.transferFamily.value();
    if (!usedQueueFamilies.contains(index)) {
      usedQueueFamilies.insert(index);
      const auto transferFamilyCreateInfo = VkDeviceQueueCreateInfo{
          .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
          .queueFamilyIndex = index,
          .queueCount = queueFamilyIndices.transferFamilyCount.value(),
          .pQueuePriorities = queueFamilyIndices.transferFamilyPriorities.data()};
      queueCreateInfo.push_back(transferFamilyCreateInfo);
    }
  }

  // Compute Queue(s) — only if different from already used
  if (queueFamilyIndices.computeFamily.has_value() &&
      queueFamilyIndices.computeFamilyCount.has_value()) {
    const uint32_t index = queueFamilyIndices.computeFamily.value();
    if (!usedQueueFamilies.contains(index)) {
      usedQueueFamilies.insert(index);
      const auto computeFamilyCreateInfo = VkDeviceQueueCreateInfo{
          .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
          .queueFamilyIndex = index,
          .queueCount = queueFamilyIndices.computeFamilyCount.value(),
          .pQueuePriorities = queueFamilyIndices.computeFamilyPriorities.data()};
      queueCreateInfo.push_back(computeFamilyCreateInfo);
    }
  }
}

void PhysicalDevice::logQueueFamilyIndices(const QueueFamilyIndices& qf) {
  auto logFamily = [](const char* name,
                      std::optional<uint32_t> index,
                      std::optional<uint32_t> count,
                      const std::vector<float>& priorities) {
    if (index.has_value()) {
      std::string prioStr;
      for (size_t i = 0; i < priorities.size(); ++i) {
        prioStr += std::to_string(priorities[i]);
        if (i + 1 < priorities.size())
          prioStr += ", ";
      }
      Log::trace("{}: index={} count={} priorities=[{}]", name, *index, *count, prioStr);
    } else {
      Log::trace("{}: not found", name);
    }
  };

  logFamily("Graphics", qf.graphicsFamily, qf.graphicsFamilyCount, qf.graphicsFamilyPriorities);
  logFamily("Compute", qf.computeFamily, qf.computeFamilyCount, qf.computeFamilyPriorities);
  logFamily("Transfer", qf.transferFamily, qf.transferFamilyCount, qf.transferFamilyPriorities);
  logFamily("Present", qf.presentFamily, qf.presentFamilyCount, qf.presentFamilyPriorities);
}

}
