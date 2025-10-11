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
class AliasRegistry;
class FrameGraph;
struct FrameGraphResult;
class FrameRenderer;
class GeometryStream;
class VirtualAllocationManager;
class BufferSystem;
class ImageSystem;
class AllocatorService;
class TransferSystem;
class GlobalBufferManager;

struct RenderContextDeps {
  Device& device;
  Swapchain& swapchain;
  IStateBuffer<SimState>& simStateBuffer;
  GeometryHandleMapper& geometryHandleMapper;
  TextureHandleMapper& textureHandleMapper;
  CommandBufferManager& commandBufferManager;
  PipelineManager& pipelineManager;
  bk::IEventQueue& eventQueue;
  AllocatorService& allocatorService;
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
  bool resizePending{false};
  std::unique_ptr<FrameManager> frameManager;
  std::unique_ptr<PerFrameUploader> perFrameUploader;
  std::unique_ptr<AliasRegistry> aliasRegistry;
  std::unique_ptr<FrameGraph> frameGraph;
  std::unique_ptr<FrameRenderer> frameRenderer;
  std::unique_ptr<GeometryStream> geometryStream;
  std::unique_ptr<VirtualAllocationManager> geometryAllocator;
  std::unique_ptr<BufferSystem> bufferSystem;
  std::unique_ptr<ImageSystem> imageSystem;
  std::unique_ptr<TransferSystem> transferSystem;
  std::unique_ptr<GlobalBufferManager> globalBufferManager;

  static void registerGeometryAliases(AliasRegistry& reg, GeometryStream& gs);
  static void registerGlobalBufferAliases(AliasRegistry& reg, GlobalBufferManager& gbm);
};

}
