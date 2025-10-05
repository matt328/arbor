#include "EntityManager.hpp"

#include "bk/Log.hpp"

namespace arb {

EntityManager::EntityManager(std::shared_ptr<bk::IEventQueue> newEventQueue,
                             IStateBuffer<SimState>& newSimStateBuffer)
    : eventQueue{std::move(newEventQueue)}, simStateBuffer(newSimStateBuffer) {
  LOG_TRACE_L1(Log::Gameplay, "Creating EntityManager");
}

EntityManager::~EntityManager() {
  LOG_TRACE_L1(Log::Gameplay, "Destroying EntityManager");
}

void EntityManager::update() {
  Timestamp currentTime = std::chrono::steady_clock::now();
  {
    auto simStateSlot = simStateBuffer.checkoutForWrite();
    if (simStateSlot.has_value()) {
      simStateSlot.value()->data = SimState{1};
    } else {
      LOG_WARNING(Log::Gameplay, "EntityManager can't checkout for write");
    }
    // FinalizerSystem::update(*registry, simStateSlot.value()->data, currentTime);
    simStateBuffer.commitWrite(simStateSlot.value());
  }
}

}
