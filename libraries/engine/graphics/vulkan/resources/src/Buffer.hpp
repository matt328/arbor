#pragma once

#include <optional>
#include <string>
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

  uint64_t validFromFrame{};
  std::optional<uint64_t> validToFrame;
};

}
