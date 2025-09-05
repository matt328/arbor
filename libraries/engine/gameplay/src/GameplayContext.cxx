#include "GameplayContext.hpp"
#include "bk/IEventQueue.hpp"

namespace arb {

auto makeGameplayContext(std::shared_ptr<bk::IEventQueue> newEventQueue)
    -> std::unique_ptr<IGameplayContext> {
  return std::make_unique<GameplayContext>(std::move(newEventQueue));
}

GameplayContext::GameplayContext(std::shared_ptr<bk::IEventQueue> newEventQueue)
    : eventQueue{std::move(newEventQueue)} {
  Log->trace("Creating GameplayContext");
}

GameplayContext::~GameplayContext() {
  Log->trace("Destroying GameplayContext");
}

auto GameplayContext::run(std::stop_token token) -> void {
  Log->trace("GameplayContext::run()");
  using clock = std::chrono::steady_clock;
  constexpr int targetHz = 2;
  constexpr auto timestep = std::chrono::nanoseconds(1'000'000'000 / targetHz);

  auto nextTick = clock::now();
  while (!token.stop_requested()) {
    auto now = clock::now();
    if (now >= nextTick) {
      Log->trace("tick()");
      eventQueue->dispatchPending();
      nextTick += timestep;
      if (now > nextTick + timestep * 10) {
        nextTick = now;
      }
    } else {
      std::this_thread::sleep_until(nextTick);
    }
  }
}

}
