#pragma once

#include "bk/NonCopyMove.hpp"

#include "core/pipeline/PipelineManager.hpp"
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
class ResourceSystem;
class AliasRegistry;
class FrameGraph;
struct FrameGraphResult;
class FrameRenderer;

class RenderContext : public NonCopyableMovable {
public:
  RenderContext(const GraphicsOptions& graphicsOptions,
                Device& newDevice,
                Swapchain& newSwapchain,
                IStateBuffer<SimState>& simStateBuffer,
                GeometryHandleMapper& newGeometryHandleMapper,
                CommandBufferManager& newCommandBufferManager,
                ResourceSystem& newResourceSystem,
                PipelineManager& newPipelineManager);
  ~RenderContext();

  void renderNextFrame();

private:
  Device& device;
  Swapchain& swapchain;
  PipelineManager& pipelineManager;
  bool resizePending{false};
  std::unique_ptr<FrameManager> frameManager;
  std::unique_ptr<PerFrameUploader> perFrameUploader;
  std::unique_ptr<AliasRegistry> aliasRegistry;
  std::unique_ptr<FrameGraph> frameGraph;
  std::unique_ptr<FrameRenderer> frameRenderer;
};

}
