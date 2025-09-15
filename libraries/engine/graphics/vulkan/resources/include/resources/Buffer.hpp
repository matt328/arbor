#pragma once

#include "core/AllocatorService.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vulkan/vulkan_core.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"
#include <vma/vk_mem_alloc.h>
#pragma clang diagnostic pop

namespace arb {

struct BufferMeta {
  VkBufferCreateInfo bufferCreateInfo;
  VmaAllocationInfo allocationInfo;
  VmaAllocationCreateInfo allocationCreateInfo;
  std::string debugName;
};

struct Buffer {
  VkBuffer vkBuffer;
  BufferMeta bufferMeta;
  VmaAllocation allocation;
  void* mappedData = nullptr;

  uint64_t generation{};
  std::unordered_map<VkSemaphore, uint64_t> pendingValues;

  static auto create(AllocatorService& allocatorService,
                     const VkBufferCreateInfo& bci,
                     const VmaAllocationCreateInfo& aci,
                     std::string name) -> std::unique_ptr<Buffer> {
    VkBuffer vkBuffer{};
    VmaAllocation vmaAllocation{};
    VmaAllocationInfo vmaAllocationInfo{};
    allocatorService.createBuffer(bci, aci, vkBuffer, vmaAllocation, &vmaAllocationInfo);

    return std::make_unique<Buffer>(Buffer{
        .vkBuffer = vkBuffer,
        .bufferMeta =
            BufferMeta{
                .bufferCreateInfo = bci,
                .allocationInfo = vmaAllocationInfo,
                .allocationCreateInfo = aci,
                .debugName = std::move(name),
            },
        .allocation = vmaAllocation,
    });
  }

  operator VkBuffer() const noexcept {
    return vkBuffer;
  }
};

}
