#include "resources/images/Image.hpp"

#include "common/DebugUtils.hpp"
#include "bk/Logger.hpp"
#include "vulkan/vulkan_core.h"

namespace arb {

Image::Image(Device* newDevice,
             AllocatorService& newAllocatorService,
             const VkImageCreateInfo& ici,
             const VmaAllocationCreateInfo& aci,
             const std::optional<std::string>& name)
    : device{newDevice},
      allocatorService{newAllocatorService},
      vkImage{VK_NULL_HANDLE},
      allocation{VK_NULL_HANDLE},
      imageCreateInfo{ici},
      allocationCreateInfo{aci},
      allocationInfo{},
      debugName{name.value_or("Unnamed Image")} {
  allocationInfo.pName = debugName.c_str();
  allocatorService.createImage(ici, aci, vkImage, allocation, debugName, &allocationInfo);
  dbg::setDebugName(*device, vkImage, debugName);
}

Image::~Image() {
  Log::trace("Destroying Image {}", debugName);
  if (vkImage != VK_NULL_HANDLE && allocation != VK_NULL_HANDLE) {
    allocatorService.destroyImage(vkImage, allocation);
    vkImage = VK_NULL_HANDLE;
    allocation = VK_NULL_HANDLE;
    allocationInfo = {};
  }
}

}
