#pragma once

#include "bk/NonCopyMove.hpp"
#include "core/AllocatorService.hpp"
#include "core/Device.hpp"

#include <string>
#include <vulkan/vulkan_core.h>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"
#include <vma/vk_mem_alloc.h>
#pragma clang diagnostic pop

namespace arb {

class Image : NonCopyableMovable {
public:
  Image(Device* newDevice,
        AllocatorService& newAllocatorService,
        const VkImageCreateInfo& ici,
        const VmaAllocationCreateInfo& aci,
        const std::optional<std::string>& name = std::nullopt);
  ~Image();

  operator VkImage() const {
    return vkImage;
  }

  [[nodiscard]] auto getExtent() const -> VkExtent3D {
    return imageCreateInfo.extent;
  }

private:
  Device* device;
  AllocatorService& allocatorService;
  VkImage vkImage;
  VmaAllocation allocation;
  VkImageCreateInfo imageCreateInfo;
  VmaAllocationCreateInfo allocationCreateInfo;
  VmaAllocationInfo allocationInfo;
  std::string debugName;
};

}
