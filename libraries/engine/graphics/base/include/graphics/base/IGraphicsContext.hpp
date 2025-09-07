#pragma once

#include "engine/common/SimState.hpp"
#include "engine/common/TripleBuffer.hpp"
#include "graphics/common/GraphicsOptions.hpp"
#include "bk/IEventQueue.hpp"
#include "bk/NativeWindowHandle.hpp"

namespace arb {

class IGraphicsContext : public NonCopyableMovable {
public:
  virtual ~IGraphicsContext() = default;
  virtual auto run(std::stop_token token) -> void = 0;
};

auto makeGraphicsContext(std::shared_ptr<bk::IEventQueue> newEventQueue,
                         IStateBuffer<SimState>& newSimStateBuffer,
                         const GraphicsOptions& newOptions,
                         bk::NativeWindowHandle newWindowHandle)
    -> std::unique_ptr<IGraphicsContext>;

}
