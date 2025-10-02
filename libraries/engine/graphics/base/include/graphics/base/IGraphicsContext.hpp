#pragma once

#include <stop_token>

#include "engine/common/IStateBuffer.hpp"
#include "engine/common/SimState.hpp"
#include "bk/IEventQueue.hpp"
#include "bk/NonCopyMove.hpp"
#include "bk/NativeWindowHandle.hpp"

namespace arb {

struct EngineOptions;

class IGraphicsContext : public NonCopyableMovable {
public:
  virtual ~IGraphicsContext() = default;
  virtual auto run(std::stop_token token) -> void = 0;
};

auto makeGraphicsContext(std::shared_ptr<bk::IEventQueue> newEventQueue,
                         IStateBuffer<SimState>& newSimStateBuffer,
                         const EngineOptions& newOptions,
                         bk::NativeWindowHandle newWindowHandle)
    -> std::unique_ptr<IGraphicsContext>;

}
