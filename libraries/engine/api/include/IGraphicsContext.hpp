#pragma once

#include "GraphicsOptions.hpp"
#include "bk/NativeWindowHandle.hpp"

namespace arb {

class IEventQueue;

class IGraphicsContext : public NonCopyableMovable {
public:
  virtual ~IGraphicsContext() = default;
  virtual auto run(std::stop_token token) -> void = 0;
};

auto makeGraphicsContext(std::shared_ptr<IEventQueue> newEventQueue,
                         const GraphicsOptions& newOptions,
                         bk::NativeWindowHandle newWindowHandle)
    -> std::unique_ptr<IGraphicsContext>;

}
