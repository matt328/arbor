#pragma once

#include <functional>

namespace Arbor {

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
struct hash<Arbor::Handle<T>> {
  auto operator()(const Arbor::Handle<T>& h) const -> size_t {
    return h.id;
  }
};

template <typename T>
struct hash<Arbor::LogicalHandle<T>> {
  auto operator()(const Arbor::LogicalHandle<T>& h) const -> size_t {
    return h.id;
  }
};

}
