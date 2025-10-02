#pragma once

#include "bk/IEventQueue.hpp"
#include "graphics/base/IGraphicsContext.hpp"
#include "common/HandleMapperTypes.hpp"

namespace arb {

class CoreContext;
class RenderContext;
class ResourceContext;
class AssetContext;

class VulkanContext : public IGraphicsContext {
public:
  VulkanContext(std::shared_ptr<bk::IEventQueue> newEventQueue,
                IStateBuffer<SimState>& newSimStateBuffer,
                const EngineOptions& newOptions,
                bk::NativeWindowHandle newWindowHandle);
  ~VulkanContext() override;

  auto run(std::stop_token token) -> void override;

private:
  std::shared_ptr<bk::IEventQueue> eventQueue;
  std::shared_ptr<CoreContext> coreContext;
  std::unique_ptr<GeometryHandleMapper> geometryHandleMapper;
  std::unique_ptr<RenderContext> renderContext;
  std::unique_ptr<ResourceContext> resourceContext;
  std::unique_ptr<AssetContext> assetContext;

  static const uint32_t MaxFrameTime = 250;
};

}
