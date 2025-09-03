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
  NonMovable& operator=(NonMovable&&) = delete;
};

struct NonCopyableMovable : NonCopyable, NonMovable {
  NonCopyableMovable() = default;
  ~NonCopyableMovable() = default;
};
