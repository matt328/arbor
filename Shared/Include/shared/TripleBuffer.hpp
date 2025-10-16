#pragma once

#include "bk/Log.hpp"

#include "IStateBuffer.hpp"

#include <array>
#include <thread>
#include <Tracy.hpp>

namespace Arbor {

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
    ZoneScoped;
    for (size_t attempt = 0; attempt < maxRetries; ++attempt) {
      if (auto slot = tryCheckoutForWrite()) {
        TracyPlot("TripleBuffer_ReadAttempts", (long long)attempt);
        return slot;
      }
      std::this_thread::sleep_for(std::chrono::microseconds(DefaultSleep));
    }
    TracyPlot("TripleBuffer_WriteFailures", (long long)1);
    return std::nullopt;
  }

  auto checkoutForRead(size_t maxRetries = DefaultMaxRetries)
      -> std::optional<typename IStateBuffer<T>::Slot*> override {
    ZoneScoped;
    for (size_t attempt = 0; attempt < maxRetries; ++attempt) {
      if (auto slot = tryCheckoutForRead()) {
        TracyPlot("TripleBuffer_ReadAttempts", (int64_t)attempt);
        return slot;
      }
      std::this_thread::sleep_for(std::chrono::microseconds(DefaultSleep));
    }
    LOG_TRACE_L1(Log::Core,
                 "Couldn't checkout for read after {} retries, returning nullopt",
                 maxRetries);
    TracyPlot("TripleBuffer_ReadFailures", (int64_t)1);
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
