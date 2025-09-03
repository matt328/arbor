#pragma once

#include "IEngineContext.hpp"

namespace arb {
class EngineContext : public IEngineContext {
public:
  EngineContext(bk::NativeWindowHandle newWindowHandle);
  ~EngineContext();

  [[nodiscard]] auto getEventQueue() const -> std::shared_ptr<IEventQueue> override;

  auto update() -> void override;

private:
  std::shared_ptr<IEventQueue> eventQueue;
  bk::NativeWindowHandle windowHandle;

  std::jthread gameThread;
  std::jthread graphicsThread;

  std::exception_ptr engineError{nullptr};

  auto requestStop() -> void;
};

}
