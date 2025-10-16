#pragma once

#include "bk/ChronoDefs.hpp"

namespace Arbor {

template <typename T, size_t Capacity, typename Snapshotter>
class StateBuffer {
public:
  StateBuffer() = default;
  ~StateBuffer() = default;

  StateBuffer(const StateBuffer&) = default;
  StateBuffer(StateBuffer&&) = delete;
  auto operator=(const StateBuffer&) -> StateBuffer& = default;
  auto operator=(StateBuffer&&) -> StateBuffer& = delete;

  struct Entry {
    Timestamp t;
    T state;
    bool valid;
  };

  auto pushState(const T& newState, Timestamp t) -> void {
    const size_t idx = writeIndex.fetch_add(1, std::memory_order_acq_rel) % Capacity;
    Snapshotter::copyInto(buffer[idx].state, newState);
    buffer[idx].t = t;
    buffer[idx].valid = true;
  }

  auto getSingleState(Timestamp t) -> std::optional<T> {
  }

  auto getStates(Timestamp t) -> std::optional<std::pair<T, T>> {
    std::array<Entry, Capacity> snapshot = buffer;

    Entry* lower = nullptr;
    Entry* upper = nullptr;

    for (size_t i = 0; i < Capacity; ++i) {
      const auto& entry = snapshot[i];
      if (!entry.valid) {
        continue;
      }
      if (entry.t <= t) {
        if ((lower == nullptr) || entry.t > lower->t) {
          lower = const_cast<Entry*>(&entry);
        }
      }
      if (entry.t >= t) {
        if ((upper == nullptr) || entry.t < upper->t) {
          upper = const_cast<Entry*>(&entry);
        }
      }
    }

    if ((lower != nullptr) && (upper != nullptr) && lower->t <= t && upper->t >= t &&
        lower->t != upper->t) {
      return std::make_pair(lower->state, upper->state);
    }

    return std::nullopt;
  }

private:
  std::array<Entry, Capacity> buffer;
  std::atomic<size_t> writeIndex = 0;
};

}
