#pragma once

#include "bk/IEventQueue.hpp"
#include "graphics/base/IGraphicsContext.hpp"

namespace arb {

class CoreContext;
class RenderContext;

class VulkanContext : public IGraphicsContext {
public:
  VulkanContext(std::shared_ptr<bk::IEventQueue> newEventQueue,
                IStateBuffer<SimState>& newSimStateBuffer,
                const GraphicsOptions& newOptions,
                bk::NativeWindowHandle newWindowHandle);
  ~VulkanContext() override;

  auto run(std::stop_token token) -> void override;

private:
  std::shared_ptr<bk::IEventQueue> eventQueue;
  std::shared_ptr<CoreContext> coreContext;
  std::unique_ptr<RenderContext> renderContext;
};

}
