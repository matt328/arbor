#pragma once

#include "bk/NonCopyMove.hpp"
#include "common/ResizePolicy.hpp"
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
        AllocatorService* newAllocatorService,
        const VkImageCreateInfo& ici,
        const VmaAllocationCreateInfo& aci,
        ResizePolicy newResizePolicy,
        const std::optional<std::string>& name = std::nullopt);

  Image(Device* newDevice,
        VkImage existingImage,
        VkExtent2D extent,
        VkFormat format,
        const std::optional<std::string>& name = std::nullopt);

  ~Image();

  operator VkImage() const {
    return vkImage;
  }

  void resize(VkExtent2D newExtent);

  [[nodiscard]] auto getExtent() const -> VkExtent3D {
    return imageCreateInfo.extent;
  }

  [[nodiscard]] auto getFormat() const -> VkFormat {
    return imageCreateInfo.format;
  }

  [[nodiscard]] auto getResizePolicy() const -> ResizePolicy {
    return resizePolicy;
  }

private:
  Device* device;
  AllocatorService* allocatorService = nullptr;
  VkImage vkImage = VK_NULL_HANDLE;
  VmaAllocation allocation = VK_NULL_HANDLE;
  VkImageCreateInfo imageCreateInfo{};
  VmaAllocationCreateInfo allocationCreateInfo{};
  VmaAllocationInfo allocationInfo{};
  ResizePolicy resizePolicy;
  std::string debugName;
  bool ownsMemory = true;
};

}
