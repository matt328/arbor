#include "EventQueue2.hpp"

namespace arb {

auto makeEventQueue() -> std::shared_ptr<IEventQueue> {
  return std::make_shared<EventQueue>();
}

}
