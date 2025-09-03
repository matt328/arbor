#pragma once

namespace arb {

class IEventQueue;

class IGameplayContext : public NonCopyableMovable {
public:
  virtual ~IGameplayContext() = default;
  virtual auto run(std::stop_token token) -> void = 0;
};

auto makeGameplayContext(std::shared_ptr<IEventQueue> newEventQueue)
    -> std::unique_ptr<IGameplayContext>;

}
