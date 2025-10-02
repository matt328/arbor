#pragma once

#include <string>

// This include is full of warnings, but it's fine everything's fine.
#include "vulkan/vulkan_core.h"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"
#include <vma/vk_mem_alloc.h>
#pragma clang diagnostic pop

#include "bk/NonCopyMove.hpp"
#include "Device.hpp"

namespace arb {

class AllocatorService : public NonCopyableMovable {
public:
  AllocatorService(VkPhysicalDevice physicalDevice, Device& newDevice, VkInstance instance);
  ~AllocatorService();

  void createBuffer(const VkBufferCreateInfo& bci,
                    const VmaAllocationCreateInfo& aci,
                    VkBuffer& outBuffer,
                    VmaAllocation& outAlloc,
                    VmaAllocationInfo* outAllocInfo = nullptr);

  void destroyBuffer(VkBuffer buffer, VmaAllocation alloc);

  [[nodiscard]] auto mapMemory(VmaAllocation alloc) -> void*;
  void unmapMemory(VmaAllocation alloc);

  auto createImage(const VkImageCreateInfo& ici,
                   const VmaAllocationCreateInfo& aci,
                   VkImage& outImage,
                   VmaAllocation& outAllocation,
                   std::string imageName,
                   VmaAllocationInfo* outAllocationInfo = nullptr) -> void;

  auto destroyImage(VkImage image, VmaAllocation allocation) -> void;

  auto getAllocationMemoryProperties(VmaAllocation allocation) -> VkMemoryPropertyFlags;

private:
  VmaAllocator allocator{VK_NULL_HANDLE};
  Device& device;
};

}
