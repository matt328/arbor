#pragma once

#include <string_view>
#include <vulkan/vulkan.h>

namespace arb::dbg {

template <typename T>
struct VulkanObjectType;

template <>
struct VulkanObjectType<VkSemaphore> {
  static constexpr VkObjectType value = VK_OBJECT_TYPE_SEMAPHORE;
};

template <>
struct VulkanObjectType<VkFence> {
  static constexpr VkObjectType value = VK_OBJECT_TYPE_FENCE;
};

template <>
struct VulkanObjectType<VkBuffer> {
  static constexpr VkObjectType value = VK_OBJECT_TYPE_BUFFER;
};

template <>
struct VulkanObjectType<VkImage> {
  static constexpr VkObjectType value = VK_OBJECT_TYPE_IMAGE;
};

template <>
struct VulkanObjectType<VkImageView> {
  static constexpr VkObjectType value = VK_OBJECT_TYPE_IMAGE_VIEW;
};

template <>
struct VulkanObjectType<VkDescriptorSetLayout> {
  static constexpr VkObjectType value = VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT;
};

template <>
struct VulkanObjectType<VkSampler> {
  static constexpr VkObjectType value = VK_OBJECT_TYPE_SAMPLER;
};

template <typename Handle>
inline void setDebugName(VkDevice device, Handle handle, std::string_view name) {
#ifdef DEBUG
  if (auto func = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(
          vkGetDeviceProcAddr(device, "vkSetDebugUtilsObjectNameEXT"))) {
    VkDebugUtilsObjectNameInfoEXT info{
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
        .objectType = VulkanObjectType<Handle>::value,
        .objectHandle = reinterpret_cast<uint64_t>(handle),
        .pObjectName = name.data(),
    };
    func(device, &info);
  }
#else
  (void)device;
  (void)handle;
  (void)name;
#endif
}

}
