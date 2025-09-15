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

struct CommandBufferUse {
  std::thread::id threadId;
  uint8_t frameId;
  size_t hashKey;
};

struct QueueConfig {
  QueueType queueType;
  std::vector<CommandBufferUse> uses;
};

struct CommandBufferInfo {
  std::vector<QueueConfig> queueConfigs;
};

class CommandBufferManager : public NonCopyableMovable {
public:
  CommandBufferManager(Device& newDevice);
  ~CommandBufferManager();

  auto allocateCommandBuffers(const CommandBufferInfo& info) -> void;
  auto requestCommandBuffer(const CommandBufferRequest& request) -> CommandBuffer&;

  /// Transfer CommandBuffers are more effective if used once and freed, so callers should let this
  /// go out of scope after it's been submitted, and request a fresh one each time.
  auto getTransferCommandBuffer() -> CommandBuffer;

private:
  Device& device;

  std::unordered_map<CommandPoolKey, VkCommandPool> poolMap;
  std::unordered_map<CommandBufferRequest, CommandBuffer> bufferMap;

  VkCommandPool transferCommandPool;
};

}
