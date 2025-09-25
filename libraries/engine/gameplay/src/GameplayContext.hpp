#pragma once

#include "bk/IEventQueue.hpp"
#include "gameplay/IGameplayContext.hpp"
#include "engine/common/IStateBuffer.hpp"
#include "engine/common/SimState.hpp"

namespace arb {

class EntityManager;

class GameplayContext : public IGameplayContext {
public:
  GameplayContext(std::shared_ptr<bk::IEventQueue> newEventQueue,
                  IStateBuffer<SimState>& newSimStateBuffer);
  ~GameplayContext();

  auto run(std::stop_token token) -> void override;

private:
  std::shared_ptr<bk::IEventQueue> eventQueue;
  IStateBuffer<SimState>& simStateBuffer;
  std::unique_ptr<EntityManager> entityManager;
};

}
