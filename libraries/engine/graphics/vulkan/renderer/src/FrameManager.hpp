#pragma once

#include "common/ImageAcquireResult.hpp"

namespace arb {

struct GraphicsOptions;
class Frame;
class Device;
class Swapchain;

class FrameManager : NonCopyableMovable {
public:
  FrameManager(const GraphicsOptions& graphicsOptions, Device& newDevice, Swapchain& swapchain);
  ~FrameManager();

  auto acquireFrame() -> std::variant<Frame*, ImageAcquireResult>;

private:
  Device& device;
  Swapchain& swapchain;
  size_t currentFrame;
  std::vector<std::unique_ptr<Frame>> frames;
};

}
