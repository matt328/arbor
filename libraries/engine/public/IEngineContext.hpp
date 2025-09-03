#pragma once

#include "bk/NativeWindowHandle.hpp"

namespace arb {

class IEventQueue;

class IEngineContext : public NonCopyableMovable {
public:
  [[nodiscard]] virtual auto getEventQueue() const -> std::shared_ptr<IEventQueue> = 0;
  virtual auto update() -> void = 0;
};

auto makeEngineContext(bk::NativeWindowHandle windowHandle) -> std::shared_ptr<IEngineContext>;

}
