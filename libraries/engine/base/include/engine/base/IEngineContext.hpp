#pragma once

#include "bk/NativeWindowHandle.hpp"
#include "bk/IEventQueue.hpp"
#include "bk/NonCopyMove.hpp"
#include "engine/common/EngineOptions.hpp"

namespace arb {

class IEngineContext : public NonCopyableMovable {
public:
  [[nodiscard]] virtual auto getEventQueue() const -> std::shared_ptr<bk::IEventQueue> = 0;
  virtual auto update() -> void = 0;
  virtual ~IEngineContext() = default;
};

auto makeEngineContext(bk::NativeWindowHandle windowHandle, EngineOptions engineOptions)
    -> std::unique_ptr<IEngineContext>;

}
