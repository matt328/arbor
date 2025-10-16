#pragma once

#include <cstdint>
#include <memory>
#include <thread>

#include "shared/NonCopyMove.hpp"
#include "shared/IEventQueue.hpp"
#include "shared/SimState.hpp"
#include "shared/IStateBuffer.hpp"

namespace Arbor {

struct EngineOptions {};

class EngineContext : public NonCopyableMovable {
public:
  explicit EngineContext(EngineOptions engineOptions);
  ~EngineContext();

private:
  std::shared_ptr<IEventQueue> eventQueue;
  std::unique_ptr<IStateBuffer<SimState>> simStateBuffer = nullptr;

  std::jthread gameThread;
  std::jthread graphicsThread;

  std::exception_ptr engineError{nullptr};

  auto requestStop() -> void;
};

}
