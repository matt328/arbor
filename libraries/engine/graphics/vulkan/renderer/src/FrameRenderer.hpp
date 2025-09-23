#pragma once

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

class FrameRenderer : public NonCopyableMovable {
public:
  FrameRenderer(Device& newDevice,
                Swapchain& newSwapchain,
                FrameManager& newFrameManager,
                PerFrameUploader& newPerFrameUploader,
                AliasRegistry& newAliasRegistry,
                FrameGraph& newFrameGraph);
  ~FrameRenderer();

  void renderNextFrame();

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
