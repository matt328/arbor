#pragma once

#include <vulkan/vulkan_core.h>

#include "bk/NonCopyMove.hpp"

namespace arb {

class CommandBuffer : public NonCopyable {
public:
  CommandBuffer(VkDevice device, VkCommandPool pool, VkCommandBuffer buf);
  ~CommandBuffer();

  CommandBuffer(CommandBuffer&& other) noexcept;
  auto operator=(CommandBuffer&& other) noexcept -> CommandBuffer&;

  operator VkCommandBuffer() const noexcept {
    return handle;
  }

  void begin(const VkCommandBufferBeginInfo& info);
  void end();
  void pipelineBarrier2(const VkDependencyInfo& dep);

private:
  VkDevice device;
  VkCommandPool pool;
  VkCommandBuffer handle;
};

}
