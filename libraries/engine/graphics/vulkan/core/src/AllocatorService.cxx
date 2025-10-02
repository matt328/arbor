#include "core/AllocatorService.hpp"

#include "bk/Logger.hpp"
#include "common/ErrorUtils.hpp"
#include "common/DebugUtils.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"
#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>
#pragma clang diagnostic pop

namespace arb {

AllocatorService::AllocatorService(VkPhysicalDevice physicalDevice,
                                   Device& newDevice,
                                   VkInstance instance)
    : device{newDevice} {
  Log::trace("Creating AllocatorService");

  constexpr auto vulkanFunctions =
      VmaVulkanFunctions{.vkGetInstanceProcAddr = vkGetInstanceProcAddr,
                         .vkGetDeviceProcAddr = vkGetDeviceProcAddr};

  const auto allocatorCreateInfo =
      VmaAllocatorCreateInfo{.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
                             .physicalDevice = physicalDevice,
                             .device = device,
                             .pVulkanFunctions = &vulkanFunctions,
                             .instance = instance};
  checkVk(vmaCreateAllocator(&allocatorCreateInfo, &allocator), "vmaCreateAllocator");
}

AllocatorService::~AllocatorService() {
  Log::trace("Destroying AllocatorService");
  vmaDestroyAllocator(allocator);
}

void AllocatorService::createBuffer(const VkBufferCreateInfo& bci,
                                    const VmaAllocationCreateInfo& aci,
                                    VkBuffer& outBuffer,
                                    VmaAllocation& outAlloc,
                                    VmaAllocationInfo* outAllocInfo) {
  checkVk(vmaCreateBuffer(allocator, &bci, &aci, &outBuffer, &outAlloc, outAllocInfo),
          "vmaCreateBuffer");
  dbg::setDebugName(device, outBuffer, "UnnamedBuffer");
}

void AllocatorService::destroyBuffer(VkBuffer buffer, VmaAllocation alloc) {
  vmaDestroyBuffer(allocator, buffer, alloc);
}

[[nodiscard]] auto AllocatorService::mapMemory(VmaAllocation alloc) -> void* {
  void* memory = nullptr;
  checkVk(vmaMapMemory(allocator, alloc, &memory), "vmaMapMemory");
  return memory;
}

void AllocatorService::unmapMemory(VmaAllocation alloc) {
  vmaUnmapMemory(allocator, alloc);
}

auto AllocatorService::createImage(const VkImageCreateInfo& ici,
                                   const VmaAllocationCreateInfo& aci,
                                   VkImage& outImage,
                                   VmaAllocation& outAllocation,
                                   std::string imageName,
                                   VmaAllocationInfo* outAllocationInfo) -> void {
  checkVk(vmaCreateImage(allocator, &ici, &aci, &outImage, &outAllocation, outAllocationInfo),
          std::format("vmaCreateImage name={}", imageName).c_str());
}

auto AllocatorService::destroyImage(VkImage image, VmaAllocation allocation) -> void {
  vmaDestroyImage(allocator, image, allocation);
}

auto AllocatorService::getAllocationMemoryProperties(VmaAllocation allocation)
    -> VkMemoryPropertyFlags {
  VkMemoryPropertyFlags flags{};
  vmaGetAllocationMemoryProperties(allocator, allocation, &flags);
  return flags;
}

}
