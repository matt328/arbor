#include "TransferSystem.hpp"

#include "bk/Log.hpp"
#include "common/ErrorUtils.hpp"
#include "common/DebugUtils.hpp"

#include "core/Device.hpp"
#include "core/command-buffers/CommandBuffer.hpp"
#include "core/command-buffers/CommandBufferManager.hpp"
#include "core/Buffer.hpp"
#include "vulkan/vulkan_core.h"

namespace arb {

TransferSystem::TransferSystem(const TransferSystemDeps& deps)
    : device{deps.device}, commandBufferManager{deps.commandBufferManager} {
  LOG_TRACE_L1(Log::Renderer, "Creating TransferSystem");
  const auto timelineInfo =
      VkSemaphoreTypeCreateInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
                                .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
                                .initialValue = 0};
  const auto createInfo = VkSemaphoreCreateInfo{
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
      .pNext = &timelineInfo,
  };
  checkVk(vkCreateSemaphore(device, &createInfo, nullptr, &copySemaphore), "vkCreateSemaphore");
  dbg::setDebugName(device, copySemaphore, "TransferQueueCopySemaphore");
}

TransferSystem::~TransferSystem() {
  LOG_TRACE_L1(Log::Renderer, "Destroying TransferSystem");
  if (copySemaphore != VK_NULL_HANDLE) {
    vkDestroySemaphore(device, copySemaphore, nullptr);
  }
}

auto TransferSystem::submitCopyBatch(const std::vector<std::tuple<Buffer&, Buffer&>>& copyPairs)
    -> uint64_t {
  if (copyPairs.empty()) {
    return getCurrentCopyValue();
  }

  const auto nextValue = ++currentValue;

  auto cmd = commandBufferManager.getTransferCommandBuffer();
  cmd.begin(VkCommandBufferBeginInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                                     .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT});

  for (const auto& [src, dst] : copyPairs) {
    auto region = VkBufferCopy2{.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
                                .srcOffset = 0,
                                .dstOffset = 0,
                                .size = std::min(src.getSize(), dst.getSize())};
    const auto copyInfo2 = VkCopyBufferInfo2{
        .sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
        .srcBuffer = src,
        .dstBuffer = dst,
        .regionCount = 1,
        .pRegions = &region,
    };
    vkCmdCopyBuffer2(cmd, &copyInfo2);
  }
  endAndSubmit(cmd, nextValue);
  return nextValue;
}

auto TransferSystem::getCurrentCopyValue() const -> uint64_t {
  return currentValue;
}

void TransferSystem::endAndSubmit(VkCommandBuffer cmd, uint64_t signalValue) {
  checkVk(vkEndCommandBuffer(cmd), "vkEndCommandBuffer");

  const auto timelineInfo = VkTimelineSemaphoreSubmitInfo{
      .signalSemaphoreValueCount = 1,
      .pSignalSemaphoreValues = &signalValue,
  };
  const auto submitInfo = VkSubmitInfo{.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                       .pNext = &timelineInfo,
                                       .commandBufferCount = 1,
                                       .pCommandBuffers = &cmd,
                                       .signalSemaphoreCount = 1,
                                       .pSignalSemaphores = &copySemaphore};
  vkQueueSubmit(device.transferQueue(), 1, &submitInfo, VK_NULL_HANDLE);
}

}
