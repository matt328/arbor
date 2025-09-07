#pragma once

#include "engine/common/IStateBuffer.hpp"
#include "engine/common/SimState.hpp"

namespace arb {

struct GraphicsOptions;
class FrameManager;
class Device;
class Swapchain;
class Frame;
class PerFrameUploader;

class RenderContext : public NonCopyableMovable {
public:
  RenderContext(const GraphicsOptions& graphicsOptions,
                Device& newDevice,
                Swapchain& newSwapchain,
                IStateBuffer<SimState>& simStateBuffer);
  ~RenderContext();

  auto renderNextFrame() -> void;

private:
  Device& device;
  Swapchain& swapchain;
  bool resizePending{false};
  std::unique_ptr<FrameManager> frameManager;
  std::unique_ptr<PerFrameUploader> perFrameUploader;

  auto recreateSwapchain() -> void;
  auto tryAcquireFrame() -> Frame*;
};

}
