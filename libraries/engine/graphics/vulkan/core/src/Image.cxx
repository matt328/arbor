#include "core/Image.hpp"

#include "common/DebugUtils.hpp"
#include "bk/Logger.hpp"
#include "vulkan/vulkan_core.h"

namespace arb {

Image::Image(Device* newDevice,
             AllocatorService* newAllocatorService,
             const VkImageCreateInfo& ici,
             const VmaAllocationCreateInfo& aci,
             const std::optional<std::string>& name)
    : device{newDevice},
      allocatorService{newAllocatorService},
      imageCreateInfo{ici},
      allocationCreateInfo{aci},
      allocationInfo{},
      debugName{name.value_or("Unnamed Image")} {
  allocationInfo.pName = debugName.c_str();
  allocatorService->createImage(ici, aci, vkImage, allocation, debugName, &allocationInfo);
  dbg::setDebugName(*device, vkImage, debugName);
}

Image::Image(Device* newDevice,
             VkImage existingImage,
             VkExtent2D extent,
             VkFormat format,
             const std::optional<std::string>& name)
    : device{newDevice}, vkImage{existingImage} {
  imageCreateInfo.extent = VkExtent3D{.width = extent.width, .height = extent.height, .depth = 1};
  imageCreateInfo.format = format;
}

Image::~Image() {
  Log::trace("Destroying Image {}", debugName);
  // If allocation == VK_NULL_HANDLE, we know it's an externally managed image
  if (vkImage != VK_NULL_HANDLE && allocation != VK_NULL_HANDLE) {
    Log::trace("Freeing Image Resources: {}", debugName);
    allocatorService->destroyImage(vkImage, allocation);
    vkImage = VK_NULL_HANDLE;
    allocation = VK_NULL_HANDLE;
    allocationInfo = {};
  }
}

}
