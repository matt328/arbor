#pragma once

struct NonCopyable {
  NonCopyable() = default;
  ~NonCopyable() = default;

  NonCopyable(const NonCopyable&) = delete;
  NonCopyable& operator=(const NonCopyable&) = delete;
};

struct NonMovable {
  NonMovable() = default;
  ~NonMovable() = default;

  NonMovable(NonMovable&&) = delete;
  auto operator=(NonMovable&&) -> NonMovable& = delete;
};

struct NonCopyableMovable : NonCopyable, NonMovable {
  NonCopyableMovable() = default;
  ~NonCopyableMovable() = default;
};
