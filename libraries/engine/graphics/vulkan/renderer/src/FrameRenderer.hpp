#pragma once

#include "IResizable.hpp"
#include "engine/common/RenderSurfaceState.hpp"
#include "bk/NonCopyMove.hpp"
#include "vulkan/vulkan_core.h"

namespace arb {

class Device;
class Swapchain;
class FrameManager;
class PerFrameUploader;
class AliasRegistry;
class FrameGraph;
class Frame;
struct FrameGraphResult;

struct FrameRendererDeps {
  Device& device;
  Swapchain& swapchain;
  FrameManager& frameManager;
  PerFrameUploader& perFrameUploader;
  AliasRegistry& aliasRegistry;
  FrameGraph& frameGraph;
};

struct FrameRendererCreateInfo {
  RenderSurfaceState renderSurfaceState;
};

class FrameRenderer : public NonCopyableMovable, IResizable {
public:
  explicit FrameRenderer(const FrameRendererDeps& deps, const FrameRendererCreateInfo& info);
  ~FrameRenderer();

  void renderNextFrame();
  void resize(const RenderSurfaceState& newState) override;

private:
  Device& device;
  Swapchain& swapchain;
  FrameManager& frameManager;
  PerFrameUploader& perFrameUploader;
  AliasRegistry& aliasRegistry;
  FrameGraph& frameGraph;

  bool resizePending{false};

  void recreateSwapchain();
  auto tryAcquireFrame() -> Frame*;
  void submitFrame(Frame* frame, const FrameGraphResult& frameResult);
  auto presentFrame(Frame* frame) -> VkResult;
};

}
