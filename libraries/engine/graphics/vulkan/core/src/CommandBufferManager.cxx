#include "core/command-buffers/CommandBufferManager.hpp"

#include "bk/Logger.hpp"
#include "core/Device.hpp"
#include "core/command-buffers/CommandBuffer.hpp"
#include "common/ErrorUtils.hpp"

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

auto CommandBufferManager::allocateCommandBuffers(const CommandBufferInfo& info) -> void {
}

auto CommandBufferManager::requestCommandBuffer(const CommandBufferRequest& request)
    -> CommandBuffer& {
}

auto CommandBufferManager::getTransferCommandBuffer() -> CommandBuffer {
}

}
