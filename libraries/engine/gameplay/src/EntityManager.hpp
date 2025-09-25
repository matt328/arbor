#pragma once

#include "bk/NonCopyMove.hpp"
#include "bk/IEventQueue.hpp"
#include "engine/common/IStateBuffer.hpp"
#include "engine/common/SimState.hpp"

namespace arb {

class EntityManager : public NonCopyableMovable {
public:
  explicit EntityManager(std::shared_ptr<bk::IEventQueue> newEventQueue,
                         IStateBuffer<SimState>& newSimStateBuffer);
  ~EntityManager();

  void update();

private:
  std::shared_ptr<bk::IEventQueue> eventQueue;
  IStateBuffer<SimState>& simStateBuffer;
};

}
