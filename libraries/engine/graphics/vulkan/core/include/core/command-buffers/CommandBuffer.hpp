#pragma once

#include <vulkan/vulkan_core.h>

#include "bk/NonCopyMove.hpp"

namespace arb {

class CommandBuffer : public NonCopyable {
public:
  CommandBuffer(VkDevice newDevice, VkCommandPool pool, VkCommandBuffer buf);
  ~CommandBuffer();

  CommandBuffer(CommandBuffer&& other) noexcept;
  auto operator=(CommandBuffer&& other) noexcept -> CommandBuffer&;

  explicit operator VkCommandBuffer() const noexcept {
    return handle;
  }

  void begin(const VkCommandBufferBeginInfo& info) const;
  void end() const;
  void pipelineBarrier2(const VkDependencyInfo& dep) const;

private:
  VkDevice device;
  VkCommandPool pool;
  VkCommandBuffer handle;
};

}
