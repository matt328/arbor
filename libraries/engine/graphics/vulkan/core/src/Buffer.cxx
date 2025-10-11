#include "core/Buffer.hpp"

#include "bk/Log.hpp"
#include "core/AllocatorService.hpp"
#include "vulkan/vulkan_core.h"

namespace arb {

Buffer::Buffer(AllocatorService& newAllocatorService,
               const VkBufferCreateInfo& bci,
               const VmaAllocationCreateInfo& aci,
               const std::optional<std::string>& debugName)
    : allocatorService{newAllocatorService} {
  LOG_TRACE_L1(Log::Core, "Creating Buffer");
  allocatorService.createBuffer(bci, aci, handle, allocation, &allocationInfo);
}

Buffer::~Buffer() {
  LOG_TRACE_L1(Log::Core, "Creating Buffer");
  if (handle != VK_NULL_HANDLE) {

    allocatorService.destroyBuffer(handle, allocation);
    handle = VK_NULL_HANDLE;
    bufferMeta = {};
    allocation = VK_NULL_HANDLE;
    allocationInfo = {};
  }
}

auto Buffer::isMappable() const -> bool {
  const auto memProps = allocatorService.getAllocationMemoryProperties(allocation);
  return (memProps & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0u;
}

void Buffer::map() {
  mappedData = allocatorService.mapMemory(allocation);
}

auto Buffer::isMapped() const -> bool {
  return mappedData != nullptr;
}

}
