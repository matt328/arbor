#include "EntityManager.hpp"

#include "bk/Logger.hpp"

namespace arb {

EntityManager::EntityManager(std::shared_ptr<bk::IEventQueue> newEventQueue,
                             IStateBuffer<SimState>& newSimStateBuffer)
    : eventQueue{std::move(newEventQueue)}, simStateBuffer(newSimStateBuffer) {
  Log::trace("Creating EntityManager");
}

EntityManager::~EntityManager() {
  Log::trace("Destroying EntityManager");
}

void EntityManager::update() {
  Timestamp currentTime = std::chrono::steady_clock::now();
  {
    auto simStateSlot = simStateBuffer.checkoutForWrite();
    if (simStateSlot.has_value()) {
      simStateSlot.value()->data = SimState{1};
    } else {
      Log::warn("EntityManager can't checkout for write");
    }
    // FinalizerSystem::update(*registry, simStateSlot.value()->data, currentTime);
    simStateBuffer.commitWrite(simStateSlot.value());
  }
}

}
