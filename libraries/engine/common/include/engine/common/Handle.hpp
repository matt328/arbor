#pragma once

#include <functional>

namespace arb {

template <typename T>
struct Handle {
  size_t id;
  constexpr auto operator==(const Handle&) const -> bool = default;
};

template <typename T>
struct LogicalHandle {
  size_t id;
  constexpr auto operator==(const LogicalHandle&) const -> bool = default;
};

}

namespace std {

template <typename T>
struct hash<arb::Handle<T>> {
  auto operator()(const arb::Handle<T>& h) const -> size_t {
    return h.id;
  }
};

template <typename T>
struct hash<arb::LogicalHandle<T>> {
  auto operator()(const arb::LogicalHandle<T>& h) const -> size_t {
    return h.id;
  }
};

}
