#pragma once

#include "bk/NonCopyMove.hpp"

#include "engine/common/IStateBuffer.hpp"
#include "engine/common/SimState.hpp"
#include "common/HandleMapperTypes.hpp"

namespace arb {

struct GraphicsOptions;
class FrameManager;
class Device;
class Swapchain;
class Frame;
class PerFrameUploader;
class FrameGraph;
class CommandBufferManager;
class ResourceFacade;

class RenderContext : public NonCopyableMovable {
public:
  RenderContext(const GraphicsOptions& graphicsOptions,
                Device& newDevice,
                Swapchain& newSwapchain,
                IStateBuffer<SimState>& simStateBuffer,
                GeometryHandleMapper& newGeometryHandleMapper,
                CommandBufferManager& newCommandBufferManager,
                ResourceFacade& newResourceFacade);
  ~RenderContext();

  auto renderNextFrame() -> void;

private:
  Device& device;
  Swapchain& swapchain;
  bool resizePending{false};
  std::unique_ptr<FrameManager> frameManager;
  std::unique_ptr<PerFrameUploader> perFrameUploader;
  std::unique_ptr<FrameGraph> frameGraph;

  auto recreateSwapchain() -> void;
  auto tryAcquireFrame() -> Frame*;
};

}
