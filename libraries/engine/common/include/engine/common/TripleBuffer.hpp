#pragma once

#include "bk/Logger.hpp"

#include "IStateBuffer.hpp"

#include <array>
#include <thread>

namespace arb {

template <typename T>
class TripleBuffer : public IStateBuffer<T> {
public:
  auto tryCheckoutForWrite() -> std::optional<typename IStateBuffer<T>::Slot*> {
    for (auto& slot : slots) {
      bool expected = false;
      if (slot.busy.compare_exchange_strong(expected, true, std::memory_order_acquire)) {
        return &slot;
      }
    }
    return std::nullopt;
  }

  auto tryCheckoutForRead() -> std::optional<typename IStateBuffer<T>::Slot*> {
    for (auto& slot : slots) {
      bool expected = false;
      if (slot.busy.compare_exchange_strong(expected, true, std::memory_order_acquire)) {
        return &slot;
      }
    }
    return std::nullopt;
  }

  auto checkoutForWrite(size_t maxRetries = DefaultMaxRetries)
      -> std::optional<typename IStateBuffer<T>::Slot*> override {
    for (size_t attempt = 0; attempt < maxRetries; ++attempt) {
      if (auto slot = tryCheckoutForWrite()) {
        return slot;
      }
      std::this_thread::sleep_for(std::chrono::microseconds(DefaultSleep));
    }
    return std::nullopt;
  }

  auto checkoutForRead(size_t maxRetries = DefaultMaxRetries)
      -> std::optional<typename IStateBuffer<T>::Slot*> override {
    for (size_t attempt = 0; attempt < maxRetries; ++attempt) {
      if (auto slot = tryCheckoutForWrite()) {
        return slot;
      }
      std::this_thread::sleep_for(std::chrono::microseconds(DefaultSleep));
    }
    Log::warn("Couldn't checkout for read after {} retries, returning nullopt", maxRetries);
    return std::nullopt;
  }

  auto commitWrite(typename IStateBuffer<T>::Slot* slot) -> void override {
    slot->busy.store(false, std::memory_order_release);
  }

  auto commitRead(typename IStateBuffer<T>::Slot* slot) -> void override {
    slot->busy.store(false, std::memory_order_release);
  }

private:
  std::array<typename IStateBuffer<T>::Slot, 3> slots;

  static constexpr size_t DefaultMaxRetries = 10;
  static constexpr size_t DefaultSleep = 500;
};

}
