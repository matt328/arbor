#pragma once

#include <limits>
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

class AllocatorService;

class Buffer {
public:
  Buffer(AllocatorService& newAllocatorService,
         const VkBufferCreateInfo& bci,
         const VmaAllocationCreateInfo& aci,
         const std::optional<std::string>& debugName);
  ~Buffer();

  operator VkBuffer() const noexcept {
    return handle;
  }

  [[nodiscard]] auto getSize() const {
    return bufferMeta.bufferCreateInfo.size;
  }

  void setSize(VkDeviceSize size) {
    bufferMeta.bufferCreateInfo.size = size;
  }

  [[nodiscard]] auto getValidFromValue() const {
    return validFromValue;
  }

  void setValidFromValue(uint64_t value) {
    validFromValue = value;
  }

  [[nodiscard]] auto getValidToValue() const {
    return validToValue;
  }

  void setValidToValue(uint64_t value) {
    validToValue = value;
  }

  [[nodiscard]] auto getMappedData() const {
    return mappedData;
  }

  [[nodiscard]] auto getDebugName() const {
    return bufferMeta.debugName;
  }

  [[nodiscard]] auto getCreateInfo() const {
    return bufferMeta.bufferCreateInfo;
  }

  [[nodiscard]] auto getAllocationCreateInfo() const {
    return bufferMeta.allocationCreateInfo;
  }

  [[nodiscard]] auto isMappable() const -> bool;

  void map();

  [[nodiscard]] auto isMapped() const -> bool;

private:
  AllocatorService& allocatorService;
  VkBuffer handle{VK_NULL_HANDLE};
  BufferMeta bufferMeta{};
  VmaAllocation allocation{VK_NULL_HANDLE};
  VmaAllocationInfo allocationInfo{};
  void* mappedData = nullptr;

  uint64_t generation{0};
  uint64_t validFromValue{0};
  uint64_t validToValue{std::numeric_limits<uint64_t>::max()};
};

}
