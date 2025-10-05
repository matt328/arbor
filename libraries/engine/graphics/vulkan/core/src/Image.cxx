#include "core/Image.hpp"

#include "Tracy.hpp"
#include "common/DebugUtils.hpp"

#include "bk/Log.hpp"
#include "common/ResizePolicy.hpp"
#include "vulkan/vulkan_core.h"

namespace arb {

Image::Image(Device* newDevice,
             AllocatorService* newAllocatorService,
             const VkImageCreateInfo& ici,
             const VmaAllocationCreateInfo& aci,
             ResizePolicy newResizePolicy,
             const std::optional<std::string>& name)
    : device{newDevice},
      allocatorService{newAllocatorService},
      imageCreateInfo{ici},
      allocationCreateInfo{aci},
      allocationInfo{},
      resizePolicy{newResizePolicy},
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
    : device{newDevice}, vkImage{existingImage}, resizePolicy{ResizePolicy::SwapchainImage} {
  imageCreateInfo.extent = VkExtent3D{.width = extent.width, .height = extent.height, .depth = 1};
  imageCreateInfo.format = format;
}

Image::~Image() {
  if (vkImage != VK_NULL_HANDLE && allocation != VK_NULL_HANDLE) {
    LOG_TRACE_L1(Log::Core, "Freeing Image Resources: {}", debugName);
    allocatorService->destroyImage(vkImage, allocation);
    vkImage = VK_NULL_HANDLE;
    allocation = VK_NULL_HANDLE;
    allocationInfo = {};
  }
}

void Image::resize(VkExtent2D newExtent) {
  if (newExtent.width == imageCreateInfo.extent.width &&
      newExtent.height == imageCreateInfo.extent.height) {
    return;
  }

  ZoneScoped;

  auto newCreateInfo = imageCreateInfo;
  newCreateInfo.extent =
      VkExtent3D{.width = newExtent.width, .height = newExtent.height, .depth = 1};

  if (vkImage != VK_NULL_HANDLE) {
    allocatorService->destroyImage(vkImage, allocation);
    vkImage = VK_NULL_HANDLE;
    allocation = VK_NULL_HANDLE;
  }
  allocatorService->createImage(newCreateInfo,
                                allocationCreateInfo,
                                vkImage,
                                allocation,
                                debugName,
                                &allocationInfo);
  dbg::setDebugName(*device, vkImage, debugName);
}

}
