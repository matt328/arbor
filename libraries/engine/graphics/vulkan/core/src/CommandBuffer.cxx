#include "core/command-buffers/CommandBuffer.hpp"

namespace arb {

CommandBuffer::CommandBuffer(VkDevice device, VkCommandPool pool, VkCommandBuffer buf)
    : device{device}, pool{pool}, handle{buf} {
}

CommandBuffer::~CommandBuffer() {
  if (handle != VK_NULL_HANDLE) {
    vkFreeCommandBuffers(device, pool, 1, &handle);
  }
}

CommandBuffer::CommandBuffer(CommandBuffer&& other) noexcept
    : device(other.device), pool(other.pool), handle(other.handle) {
  other.handle = VK_NULL_HANDLE;
}

auto CommandBuffer::operator=(CommandBuffer&& other) noexcept -> CommandBuffer& {
  if (this != &other) {
    if (handle != VK_NULL_HANDLE) {
      vkFreeCommandBuffers(device, pool, 1, &handle);
    }
    device = other.device;
    pool = other.pool;
    handle = other.handle;
    other.handle = VK_NULL_HANDLE;
  }
  return *this;
}

void CommandBuffer::begin(const VkCommandBufferBeginInfo& info) {
  vkBeginCommandBuffer(handle, &info);
}

void CommandBuffer::end() {
  vkEndCommandBuffer(handle);
}

void CommandBuffer::pipelineBarrier2(const VkDependencyInfo& dep) {
  vkCmdPipelineBarrier2(handle, &dep);
}

}
