#pragma once

#include "base/IEngineContext.hpp"
#include "bk/IEventQueue.hpp"

namespace arb {
class EngineContext : public IEngineContext {
public:
  EngineContext(bk::NativeWindowHandle newWindowHandle);
  ~EngineContext();

  [[nodiscard]] auto getEventQueue() const -> std::shared_ptr<bk::IEventQueue> override;

  auto update() -> void override;

private:
  std::shared_ptr<bk::IEventQueue> eventQueue;
  bk::NativeWindowHandle windowHandle;

  std::jthread gameThread;
  std::jthread graphicsThread;

  std::exception_ptr engineError{nullptr};

  auto requestStop() -> void;
};

}
