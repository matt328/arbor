#pragma once

#include "bk/IEventQueue.hpp"
#include "bk/NonCopyMove.hpp"
#include "engine/common/IStateBuffer.hpp"
#include "engine/common/SimState.hpp"

namespace arb {

class IGameplayContext : public NonCopyableMovable {
public:
  virtual ~IGameplayContext() = default;
  virtual auto run(std::stop_token token) -> void = 0;
};

auto makeGameplayContext(std::shared_ptr<bk::IEventQueue> newEventQueue,
                         IStateBuffer<SimState>& newSimStateBuffer)
    -> std::unique_ptr<IGameplayContext>;

}
