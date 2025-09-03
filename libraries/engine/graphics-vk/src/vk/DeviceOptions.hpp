#include <algorithm>

#pragma once

namespace arb {

struct QueueRequirement {
  VkQueueFlagBits flags;
  uint32_t count;
};

struct EmptyFeatureChain {
  VkPhysicalDeviceFeatures2 features{};
  VkPhysicalDeviceVulkan12Features vk12{};
  VkPhysicalDeviceShaderDrawParametersFeatures drawParams{};
  VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRendering{};
  VkPhysicalDeviceExtendedDynamicStateFeaturesEXT extDyn{};
  VkPhysicalDeviceSynchronization2Features sync2{};

  EmptyFeatureChain() {
    // Root
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &vk12;

    // Vulkan 1.2 Features
    vk12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    vk12.pNext = &drawParams;

    // Shader Draw Parameters
    drawParams.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES;
    drawParams.pNext = &dynamicRendering;

    // Dynamic Rendering
    dynamicRendering.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
    dynamicRendering.pNext = &extDyn;

    // Extended Dynamic State
    extDyn.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT;
    extDyn.pNext = &sync2;

    // Synchronization2
    sync2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
  }

  // Convenient accessor for Vulkan calls
  auto root() -> VkPhysicalDeviceFeatures2* {
    return &features;
  }
};

struct FeatureChain {
  VkPhysicalDeviceFeatures2 features{};
  VkPhysicalDeviceVulkan12Features vk12{};
  VkPhysicalDeviceShaderDrawParametersFeatures drawParams{};
  VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRendering{};
  VkPhysicalDeviceExtendedDynamicStateFeaturesEXT extDyn{};
  VkPhysicalDeviceSynchronization2Features sync2{};

  FeatureChain() {
    // Root
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.features.samplerAnisotropy = VK_TRUE;
    features.pNext = &vk12;

    // Vulkan 1.2 Features
    vk12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    vk12.drawIndirectCount = VK_TRUE;
    vk12.bufferDeviceAddress = VK_TRUE;
    vk12.descriptorBindingPartiallyBound = VK_TRUE;
    vk12.runtimeDescriptorArray = VK_TRUE;
    vk12.descriptorIndexing = VK_TRUE;
    vk12.pNext = &drawParams;

    // Shader Draw Parameters
    drawParams.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES;
    drawParams.shaderDrawParameters = VK_TRUE;
    drawParams.pNext = &dynamicRendering;

    // Dynamic Rendering
    dynamicRendering.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
    dynamicRendering.dynamicRendering = VK_TRUE;
    dynamicRendering.pNext = &extDyn;

    // Extended Dynamic State
    extDyn.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT;
    extDyn.extendedDynamicState = VK_TRUE;
    extDyn.pNext = &sync2;

    // Synchronization2
    sync2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
    sync2.synchronization2 = VK_TRUE;
  }

  // Convenient accessor for Vulkan calls
  auto root() -> VkPhysicalDeviceFeatures2* {
    return &features;
  }
};

struct DeviceOptions {
  static constexpr auto RequiredExtensions =
      std::array<const char*, 10>{{VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                                   VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
                                   VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
                                   VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
                                   VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
                                   VK_EXT_EXTENDED_DYNAMIC_STATE_3_EXTENSION_NAME,
                                   VK_EXT_DESCRIPTOR_BUFFER_EXTENSION_NAME,
                                   VK_KHR_DEVICE_GROUP_EXTENSION_NAME,
                                   VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
                                   VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME}};

  static auto requiredQueues() -> std::vector<QueueRequirement> {
    return {{
        QueueRequirement{.flags = VK_QUEUE_GRAPHICS_BIT, .count = 1},
        QueueRequirement{.flags = VK_QUEUE_COMPUTE_BIT, .count = 1},
        QueueRequirement{.flags = VK_QUEUE_TRANSFER_BIT, .count = 1},
    }};
  }

  static auto supports(VkPhysicalDevice handle, VkSurfaceKHR surfaceHandle) -> bool {
    // check extension support
    uint32_t extCount = 0;
    vkEnumerateDeviceExtensionProperties(handle, nullptr, &extCount, nullptr);
    std::vector<VkExtensionProperties> available(extCount);
    vkEnumerateDeviceExtensionProperties(handle, nullptr, &extCount, available.data());

    auto properties = VkPhysicalDeviceProperties{};
    vkGetPhysicalDeviceProperties(handle, &properties);
    auto* deviceName = properties.deviceName;

    for (const auto* req : RequiredExtensions) {
      auto found = std::ranges::any_of(available, [&](const VkExtensionProperties& ext) {
        return std::strcmp(ext.extensionName, req) == 0;
      });
      if (!found) {
        Log->warn("Device {}, Required Extension {} not present", deviceName, req);
        return false;
      }
    }
    Log->trace("Extensions Ok");

    auto checkFeature = [&deviceName](bool required, bool supported, const char* name) -> bool {
      if (required && !supported) {
        Log->warn("Physical device {} missing required feature: {}", deviceName, name);
        return false;
      }
      return true;
    };

    // Check Features
    auto* supported = EmptyFeatureChain{}.root();
    vkGetPhysicalDeviceFeatures2(handle, supported);

    auto chain = FeatureChain{};
    auto* required = chain.root();

    // Vulkan 1.2 features
    auto* req12 = reinterpret_cast<VkPhysicalDeviceVulkan12Features*>(required->pNext);
    auto* sup12 = reinterpret_cast<VkPhysicalDeviceVulkan12Features*>(supported->pNext);
    if (!checkFeature(req12->drawIndirectCount != 0u,
                      sup12->drawIndirectCount != 0u,
                      "drawIndirectCount")) {
      return false;
    }
    if (!checkFeature(req12->bufferDeviceAddress != 0u,
                      sup12->bufferDeviceAddress != 0u,
                      "bufferDeviceAddress")) {
      return false;
    }
    if (!checkFeature(req12->descriptorBindingPartiallyBound != 0u,
                      sup12->descriptorBindingPartiallyBound != 0u,
                      "descriptorBindingPartiallyBound")) {
      return false;
    }
    if (!checkFeature(req12->runtimeDescriptorArray != 0u,
                      sup12->runtimeDescriptorArray != 0u,
                      "runtimeDescriptorArray")) {
      return false;
    }
    if (!checkFeature(req12->descriptorIndexing != 0u,
                      sup12->descriptorIndexing != 0u,
                      "descriptorIndexing")) {
      return false;
    }

    // Shader Draw Parameters
    auto* reqDraw = reinterpret_cast<VkPhysicalDeviceShaderDrawParametersFeatures*>(req12->pNext);
    auto* supDraw = reinterpret_cast<VkPhysicalDeviceShaderDrawParametersFeatures*>(sup12->pNext);
    if (!checkFeature(reqDraw->shaderDrawParameters != 0u,
                      supDraw->shaderDrawParameters != 0u,
                      "shaderDrawParameters")) {
      return false;
    }

    // Dynamic Rendering
    auto* reqDyn = reinterpret_cast<VkPhysicalDeviceDynamicRenderingFeaturesKHR*>(reqDraw->pNext);
    auto* supDyn = reinterpret_cast<VkPhysicalDeviceDynamicRenderingFeaturesKHR*>(supDraw->pNext);
    if (!checkFeature(reqDyn->dynamicRendering != 0u,
                      supDyn->dynamicRendering != 0u,
                      "dynamicRendering")) {
      return false;
    }

    // Extended Dynamic State
    auto* reqExtDyn =
        reinterpret_cast<VkPhysicalDeviceExtendedDynamicStateFeaturesEXT*>(reqDyn->pNext);
    auto* supExtDyn =
        reinterpret_cast<VkPhysicalDeviceExtendedDynamicStateFeaturesEXT*>(supDyn->pNext);
    if (!checkFeature(reqExtDyn->extendedDynamicState != 0u,
                      supExtDyn->extendedDynamicState != 0u,
                      "extendedDynamicState")) {
      return false;
    }

    // Synchronization2
    auto* reqSync2 = reinterpret_cast<VkPhysicalDeviceSynchronization2Features*>(reqExtDyn->pNext);
    auto* supSync2 = reinterpret_cast<VkPhysicalDeviceSynchronization2Features*>(supExtDyn->pNext);
    if (!checkFeature(reqSync2->synchronization2 != 0u,
                      supSync2->synchronization2 != 0u,
                      "synchronization2")) {
      return false;
    }

    Log->trace("Features Ok");

    // Check for queues
    auto queueRequirements = requiredQueues();
    std::vector<uint32_t> remainingCounts;
    remainingCounts.reserve(queueRequirements.size());

    for (const auto& req : queueRequirements) {
      remainingCounts.push_back(req.count);
    }

    uint32_t familyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(handle, &familyCount, nullptr);
    std::vector<VkQueueFamilyProperties> families(familyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(handle, &familyCount, families.data());

    for (const auto& f : families) {
      if (f.queueCount == 0) {
        continue;
      }

      for (size_t i = 0; i < queueRequirements.size(); ++i) {
        if (remainingCounts[i] == 0) {
          continue;
        }

        if ((f.queueFlags & queueRequirements[i].flags) != 0) {
          remainingCounts[i] =
              (remainingCounts[i] > f.queueCount) ? remainingCounts[i] - f.queueCount : 0;
        }
      }
    }

    bool allQueuesSatisfied =
        std::ranges::all_of(remainingCounts, [](uint32_t count) { return count == 0; });

    return allQueuesSatisfied;
  }
};

}
