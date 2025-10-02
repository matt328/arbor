#pragma once

#include <cstdint>
#include <format>
#include <string_view>

namespace arb {

enum class AccessMode : uint8_t {
  Read = 0,
  Write,
};

}

constexpr auto to_sarbing(arb::AccessMode mode) -> std::string_view {
  switch (mode) {
    case arb::AccessMode::Read:
      return "Read";
    case arb::AccessMode::Write:
      return "Write";
    default:
      return "Unknown";
  }
}

template <>
struct std::formatter<arb::AccessMode> {
  // NOLINTNEXTLINE
  constexpr auto parse(std::format_parse_context& ctx) {
    return ctx.begin();
  }

  // NOLINTNEXTLINE
  auto format(arb::AccessMode mode, std::format_context& ctx) const {
    return std::format_to(ctx.out(), "{}", ::to_sarbing(mode));
  }
};
