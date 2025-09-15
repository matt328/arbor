#pragma once

#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "barriers/BarrierPrecursorPlan.hpp"
#include "bk/NonCopyMove.hpp"
#include "resources/ImageHandle.hpp"

#include "barriers/LastImageUse.hpp"
#include "ComponentIds.hpp"

namespace arb {

class CommandBufferManager;
class IRenderPass;
class ResourceFacade;
class Frame;
class AliasRegistry;

struct FrameGraphResult {
  std::vector<VkCommandBuffer> commandBuffers;
};

class FrameGraph : public NonCopyableMovable {
public:
  FrameGraph(CommandBufferManager& newCommandBufferManager,
             ResourceFacade& newResourceFacade,
             AliasRegistry& newAliasRegistry);
  ~FrameGraph();

  auto addPass(std::unique_ptr<IRenderPass>&& pass) -> void;
  [[nodiscard]] auto getPass(PassId id) -> std::unique_ptr<IRenderPass>&;
  auto bake() -> void;
  auto execute(Frame* frame) -> FrameGraphResult;
  auto getSwapchainImageLastUse(ImageHandle handle) -> std::optional<LastImageUse>;
  auto setSwapchainImageLastUse(ImageHandle handle, LastImageUse lastImageUse) -> void;
  auto resetSwapchainUses() -> void;

private:
  CommandBufferManager& commandBufferManager;
  ResourceFacade& resourceFacade;
  AliasRegistry& aliasRegistry;

  std::vector<std::unique_ptr<IRenderPass>> renderPasses;
  std::unordered_map<PassId, size_t> passesById;
  BarrierPrecursorPlan barrierPrecursorPlan;

  std::unordered_map<ImageHandle, LastImageUse> swapchainLastUses;
};

}
