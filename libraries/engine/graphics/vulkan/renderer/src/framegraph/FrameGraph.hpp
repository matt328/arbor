#pragma once

#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "barriers/BarrierPrecursorPlan.hpp"
#include "bk/NonCopyMove.hpp"
#include "core/pipeline/PipelineManager.hpp"
#include "resources/images/ImageHandle.hpp"

#include "barriers/LastImageUse.hpp"
#include "ComponentIds.hpp"

namespace arb {

class CommandBufferManager;
class IRenderPass;
class ResourceSystem;
class Frame;
class AliasRegistry;

struct FrameGraphResult {
  std::vector<VkCommandBuffer> commandBuffers;
};

class FrameGraph : public NonCopyableMovable {
public:
  FrameGraph(CommandBufferManager& newCommandBufferManager,
             PipelineManager& newPipelineManager,
             AliasRegistry& newAliasRegistry);
  ~FrameGraph();

  auto execute(Frame* frame) -> FrameGraphResult;
  auto getSwapchainImageLastUse(ImageHandle handle) -> std::optional<LastImageUse>;
  void setSwapchainImageLastUse(ImageHandle handle, LastImageUse lastImageUse);
  void resetSwapchainUses();

private:
  CommandBufferManager& commandBufferManager;
  PipelineManager& pipelineManager;
  AliasRegistry& aliasRegistry;

  std::vector<std::unique_ptr<IRenderPass>> renderPasses;
  BarrierPrecursorPlan barrierPrecursorPlan;

  std::unordered_map<ImageHandle, LastImageUse> swapchainLastUses;

  void compileResources();
  void bake();
};

}
