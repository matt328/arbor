#pragma once

#include "IGameplayContext.hpp"

namespace arb {

class IEventQueue;

class GameplayContext : public IGameplayContext {
public:
  GameplayContext(std::shared_ptr<IEventQueue> newEventQueue);
  ~GameplayContext();

  auto run(std::stop_token token) -> void override;

private:
  std::shared_ptr<IEventQueue> eventQueue;
};

}
