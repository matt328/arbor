#include "GameplayContext.hpp"

#include "bk/Logger.hpp"
#include "bk/IEventQueue.hpp"
#include "engine/common/IStateBuffer.hpp"
#include "engine/common/SimState.hpp"

#include "EntityManager.hpp"

#include <Tracy.hpp>

namespace arb {

auto makeGameplayContext(std::shared_ptr<bk::IEventQueue> newEventQueue,
                         IStateBuffer<SimState>& newSimStateBuffer)
    -> std::unique_ptr<IGameplayContext> {
  return std::make_unique<GameplayContext>(std::move(newEventQueue), newSimStateBuffer);
}

GameplayContext::GameplayContext(std::shared_ptr<bk::IEventQueue> newEventQueue,
                                 IStateBuffer<SimState>& newSimStateBuffer)
    : eventQueue{std::move(newEventQueue)}, simStateBuffer(newSimStateBuffer) {
  Log::trace("Creating GameplayContext");
  entityManager = std::make_unique<EntityManager>(eventQueue, simStateBuffer);
}

GameplayContext::~GameplayContext() {
  Log::trace("Destroying GameplayContext");
}

void GameplayContext::run(std::stop_token token) {
  using clock = std::chrono::steady_clock;
  constexpr int targetHz = 120;
  constexpr auto timestep = std::chrono::nanoseconds(1'000'000'000 / targetHz);

  auto nextTick = clock::now();

  while (!token.stop_requested()) {
    FrameMarkStart("Gameplay");
    ZoneScopedN("Gameplay Loop");
    auto now = clock::now();

    int64_t steps = 0;
    while (now >= nextTick && steps < 5) { // cap catch-up
      ZoneScopedN("Gameplay Tick");
      eventQueue->dispatchPending();
      entityManager->update();
      nextTick += timestep;
      steps++;
    }
    TracyPlot("PhysicsTicksPerLoop", steps);

    const long long overshoot =
        std::chrono::duration_cast<std::chrono::microseconds>(clock::now() - nextTick).count();
    // overshoot < 0, good. overshoot > 0, the frame took too long
    TracyPlot("PhysicsOvershoot_us", (long long)overshoot);

    {
      ZoneScopedN("Sleep");
      std::this_thread::sleep_until(nextTick);
    }
    FrameMarkEnd("Gameplay");
  }
}

}
