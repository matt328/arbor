#pragma once

#include <cstdint>
#include <tuple>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace arb {

struct Buffer;
class Device;
class CommandBufferManager;

struct TransferSystemDeps {
  Device& device;
  CommandBufferManager& commandBufferManager;
};

class TransferSystem {
public:
  explicit TransferSystem(const TransferSystemDeps& deps);
  ~TransferSystem();

  auto submitCopyBatch(const std::vector<std::tuple<Buffer&, Buffer&>>& copyPairs) -> uint64_t;
  [[nodiscard]] auto getCurrentCopyValue() const -> uint64_t;

private:
  Device& device;
  CommandBufferManager& commandBufferManager;
  VkSemaphore copySemaphore{VK_NULL_HANDLE};
  uint64_t currentValue{0};

  void endAndSubmit(VkCommandBuffer cmd, uint64_t signalValue);
};

}
