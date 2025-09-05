#pragma once

#include "bk/IEventQueue.hpp"
#include "gameplay/IGameplayContext.hpp"

namespace arb {

class GameplayContext : public IGameplayContext {
public:
  GameplayContext(std::shared_ptr<bk::IEventQueue> newEventQueue);
  ~GameplayContext();

  auto run(std::stop_token token) -> void override;

private:
  std::shared_ptr<bk::IEventQueue> eventQueue;
};

}
