#pragma once

#include "engine/base/IEngineContext.hpp"
#include "bk/IEventQueue.hpp"
#include "engine/common/SimState.hpp"
#include "engine/common/IStateBuffer.hpp"

namespace arb {

class EngineContext : public IEngineContext {
public:
  EngineContext(bk::NativeWindowHandle newWindowHandle, EngineOptions engineOptions);
  ~EngineContext() override;

  [[nodiscard]] auto getEventQueue() const -> std::shared_ptr<bk::IEventQueue> override;

  auto update() -> void override;

private:
  std::shared_ptr<bk::IEventQueue> eventQueue;

  std::unique_ptr<IStateBuffer<SimState>> simStateBuffer;

  bk::NativeWindowHandle windowHandle;

  std::jthread gameThread;
  std::jthread graphicsThread;

  std::exception_ptr engineError{nullptr};

  auto requestStop() -> void;
};

}
