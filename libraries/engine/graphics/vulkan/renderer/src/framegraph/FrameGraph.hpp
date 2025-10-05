#pragma once

#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "engine/common/RenderSurfaceState.hpp"
#include "barriers/BarrierPrecursorPlan.hpp"
#include "bk/NonCopyMove.hpp"
#include "core/pipeline/PipelineManager.hpp"
#include "core/ImageHandle.hpp"

#include "barriers/LastImageUse.hpp"
#include "core/IResizable.hpp"

namespace arb {

class CommandBufferManager;
class IRenderPass;
class ResourceSystem;
class Frame;
class AliasRegistry;

struct FrameGraphResult {
  std::vector<VkCommandBuffer> commandBuffers;
};

struct FrameGraphDeps {
  CommandBufferManager& commandBufferManager;
  PipelineManager& pipelineManager;
  AliasRegistry& aliasRegistry;
};

struct FrameGraphConfig {
  RenderSurfaceState initialSurfaceState;
};

class FrameGraph : public NonCopyableMovable, IResizable {
public:
  FrameGraph(const FrameGraphDeps& deps, const FrameGraphConfig& config);
  ~FrameGraph();

  auto execute(Frame* frame) -> FrameGraphResult;
  auto getSwapchainImageLastUse(ImageHandle handle) -> std::optional<LastImageUse>;
  void setSwapchainImageLastUse(ImageHandle handle, LastImageUse lastImageUse);
  void resetSwapchainUses();
  void resize(const RenderSurfaceState& newState) override;

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
