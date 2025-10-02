#include "core/command-buffers/CommandBufferManager.hpp"

#include <cassert>
#include <set>

#include "bk/Logger.hpp"
#include "core/Device.hpp"
#include "core/command-buffers/CommandBuffer.hpp"
#include "common/ErrorUtils.hpp"
#include "vulkan/vulkan_core.h"

namespace arb {

CommandBufferManager::CommandBufferManager(Device& newDevice) : device{newDevice} {
  Log::trace("Constructing CommandBufferManager");
  const auto transferCommandPoolCreateInfo =
      VkCommandPoolCreateInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                              .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT |
                                       VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                              .queueFamilyIndex = device.getTransferQueueFamily()};

  checkVk(
      vkCreateCommandPool(device, &transferCommandPoolCreateInfo, nullptr, &transferCommandPool),
      "vkCreateCommandPool");
}

CommandBufferManager::~CommandBufferManager() {
  Log::trace("Destroying CommandBufferManager");
  vkDestroyCommandPool(device, transferCommandPool, nullptr);

  bufferMap.clear();

  for (auto& [key, pool] : poolMap) {
    vkDestroyCommandPool(device, pool, nullptr);
  }
  poolMap.clear();
}

auto CommandBufferManager::requestCommandBuffer(const CommandBufferRequest& request)
    -> CommandBuffer& {
  if (bufferMap.contains(request)) {
    return bufferMap.at(request);
  }

  const auto poolKey = CommandPoolKey{.threadId = request.threadId,
                                      .frameId = request.frameId,
                                      .queueType = request.queueType};
  // Find or create a Pool
  auto* pool = VkCommandPool{VK_NULL_HANDLE};
  if (poolMap.contains(poolKey)) {
    pool = poolMap.at(poolKey);
  } else {
    const auto queueFamilyIndex = [&]() {
      switch (request.queueType) {
        case QueueType::Graphics:
          return device.getGraphicsQueueFamily();
        case QueueType::Compute:
          return device.getComputeQueueFamily();
        case QueueType::Transfer:
          return device.getTransferQueueFamily();
      }
      throw cpptrace::logic_error("Unknown QueueType Requested");
    }();
    const auto poolCreateInfo = VkCommandPoolCreateInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = queueFamilyIndex,
    };

    checkVk(vkCreateCommandPool(device, &poolCreateInfo, nullptr, &pool), "vkCreateCommandPool");
    poolMap.emplace(poolKey, pool);
  }
  assert(pool != VK_NULL_HANDLE);
  // Allocate a command buffer from the pool
  const auto allocInfo = VkCommandBufferAllocateInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .commandPool = pool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = 1,
  };
  VkCommandBuffer cmdBuffer{VK_NULL_HANDLE};
  checkVk(vkAllocateCommandBuffers(device, &allocInfo, &cmdBuffer), "vkAllocateCommandBuffers");

  bufferMap.emplace(request, CommandBuffer{device, poolMap.at(poolKey), cmdBuffer});
  return bufferMap.at(request);
}

auto CommandBufferManager::getTransferCommandBuffer() -> CommandBuffer {
}

}
