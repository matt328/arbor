#pragma once

// This include is full of warnings, but it's fine everything's fine.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"
#include <vma/vk_mem_alloc.h>
#pragma clang diagnostic pop

#include "bk/NonCopyMove.hpp"

namespace arb {

class AllocatorService : public NonCopyableMovable {
public:
  AllocatorService(VkPhysicalDevice physicalDevice, VkDevice device, VkInstance instance);
  ~AllocatorService();

  void createBuffer(const VkBufferCreateInfo& bci,
                    const VmaAllocationCreateInfo& aci,
                    VkBuffer& outBuffer,
                    VmaAllocation& outAlloc,
                    VmaAllocationInfo* outAllocInfo = nullptr);

  void destroyBuffer(VkBuffer buffer, VmaAllocation alloc);

  [[nodiscard]] auto mapMemory(VmaAllocation alloc) -> void*;
  void unmapMemory(VmaAllocation alloc);

private:
  VmaAllocator allocator{VK_NULL_HANDLE};
};

}
