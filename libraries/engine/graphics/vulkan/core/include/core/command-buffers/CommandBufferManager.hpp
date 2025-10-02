#pragma once

#include <unordered_map>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "bk/NonCopyMove.hpp"
#include "CommandBufferRequest.hpp"
#include "CommandPoolKey.hpp"

namespace arb {

class Device;
class IDebugManager;
class CommandBuffer;

using CommandBufferHandle = size_t;

class CommandBufferManager : public NonCopyableMovable {
public:
  explicit CommandBufferManager(Device& newDevice);
  ~CommandBufferManager();

  auto requestCommandBuffer(const CommandBufferRequest& request) -> CommandBuffer&;

  auto getTransferCommandBuffer() -> CommandBuffer;

private:
  Device& device;

  std::unordered_map<CommandPoolKey, VkCommandPool> poolMap;
  std::unordered_map<CommandBufferRequest, CommandBuffer> bufferMap;

  VkCommandPool transferCommandPool;
};

}
