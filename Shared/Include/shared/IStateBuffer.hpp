#pragma once

#include "ChronoDefs.hpp"

namespace Arbor {

constexpr size_t DefaultMaxRetries = 10;

template <typename T>
class IStateBuffer {
public:
  IStateBuffer() = default;
  virtual ~IStateBuffer() = default;

  IStateBuffer(const IStateBuffer&) = default;
  IStateBuffer(IStateBuffer&&) = delete;
  auto operator=(const IStateBuffer&) -> IStateBuffer& = default;
  auto operator=(IStateBuffer&&) -> IStateBuffer& = delete;

  struct Slot {
    T data;
    Timestamp timestamp;
    size_t tag;
    std::atomic<bool> busy{false};
  };

  virtual auto checkoutForWrite(size_t maxRetries = DefaultMaxRetries) -> std::optional<Slot*> = 0;
  virtual auto checkoutForRead(size_t maxRetries = DefaultMaxRetries) -> std::optional<Slot*> = 0;

  virtual void commitWrite(Slot* slot) = 0;
  virtual void commitRead(Slot* slot) = 0;
};

}
