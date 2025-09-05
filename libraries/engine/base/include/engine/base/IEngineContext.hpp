#pragma once

#include "bk/NativeWindowHandle.hpp"
#include "bk/IEventQueue.hpp"

namespace arb {

class IEngineContext : public NonCopyableMovable {
public:
  [[nodiscard]] virtual auto getEventQueue() const -> std::shared_ptr<bk::IEventQueue> = 0;
  virtual auto update() -> void = 0;
};

auto makeEngineContext(bk::NativeWindowHandle windowHandle) -> std::shared_ptr<IEngineContext>;

}
