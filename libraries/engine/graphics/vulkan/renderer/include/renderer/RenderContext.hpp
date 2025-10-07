#pragma once

#include "bk/IEventQueue.hpp"
#include "bk/NonCopyMove.hpp"

#include "core/pipeline/PipelineManager.hpp"
#include "engine/common/IStateBuffer.hpp"
#include "engine/common/SimState.hpp"
#include "common/HandleMapperTypes.hpp"

namespace arb {

struct EngineOptions;
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
class GeometryStream;

struct RenderContextDeps {
  Device& device;
  Swapchain& swapchain;
  IStateBuffer<SimState>& simStateBuffer;
  GeometryHandleMapper& geometryHandleMapper;
  CommandBufferManager& commandBufferManager;
  ResourceSystem& resourceSystem;
  PipelineManager& pipelineManager;
  bk::IEventQueue& eventQueue;
};

class RenderContext : public NonCopyableMovable {
public:
  RenderContext(const EngineOptions& options, const RenderContextDeps& deps);
  ~RenderContext();

  void renderNextFrame();

private:
  Device& device;
  Swapchain& swapchain;
  PipelineManager& pipelineManager;
  ResourceSystem& resourceSystem;
  bool resizePending{false};
  std::unique_ptr<FrameManager> frameManager;
  std::unique_ptr<PerFrameUploader> perFrameUploader;
  std::unique_ptr<AliasRegistry> aliasRegistry;
  std::unique_ptr<FrameGraph> frameGraph;
  std::unique_ptr<FrameRenderer> frameRenderer;
  std::unique_ptr<GeometryStream> geometryStream;

  static void registerGeometryAliases(AliasRegistry& aliasRegistry, GeometryStream& geometryStream);
};

}
