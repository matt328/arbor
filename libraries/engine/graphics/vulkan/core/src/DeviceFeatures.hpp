#pragma once

#include <vulkan/vulkan_core.h>

namespace arb {

struct DeviceFeatures {
  VkPhysicalDeviceFeatures2 features2{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
  VkPhysicalDeviceVulkan12Features vk12{.sType =
                                            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
  VkPhysicalDeviceShaderDrawParametersFeatures drawParams{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES};
  VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRendering{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR};
  VkPhysicalDeviceExtendedDynamicStateFeaturesEXT extDyn{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT};
  VkPhysicalDeviceSynchronization2Features sync2{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES};

  DeviceFeatures() {
    features2.pNext = &vk12;
    vk12.pNext = &drawParams;
    drawParams.pNext = &dynamicRendering;
    dynamicRendering.pNext = &extDyn;
    extDyn.pNext = &sync2;
  }
};

}
