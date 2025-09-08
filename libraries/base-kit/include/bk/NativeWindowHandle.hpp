#pragma once

#include <cstdint>
#include <type_traits>

namespace bk {

struct NativeWindowHandle {
  std::uintptr_t handle = 0;

  template <typename T>
  void set(T nativeHandle) {
    if constexpr (std::is_pointer_v<T>) {
      handle = reinterpret_cast<std::uintptr_t>(nativeHandle);
    } else {
      handle = static_cast<std::uintptr_t>(nativeHandle);
    }
  }

  template <typename T>
  auto get() const -> T {
    if constexpr (std::is_pointer_v<T>) {
      return reinterpret_cast<T>(handle);
    } else {
      return static_cast<T>(handle);
    }
  }
};

}
