#pragma once

#include <cstdint>
#include <format>

namespace arb {

enum class PassId : uint8_t {
  Culling = 0,
  Forward,
  Composition,
  PostProcessing,
  Present,
};

enum class DispatcherId : uint8_t {
  Culling = 0,
  Cube,
  Composition,
};

}

template <>
struct std::formatter<arb::PassId> {
  // NOLINTNEXTLINE
  constexpr auto parse(std::format_parse_context& ctx) {
    return ctx.begin();
  }

  // NOLINTNEXTLINE
  auto format(arb::PassId id, std::format_context& ctx) const {
    std::string_view name = "Unknown";
    switch (id) {
      case arb::PassId::Culling:
        name = "Culling";
        break;
      case arb::PassId::Forward:
        name = "Forward";
        break;
      case arb::PassId::Composition:
        name = "Composition";
        break;
      case arb::PassId::PostProcessing:
        name = "PostProcessing";
        break;
      case arb::PassId::Present:
        name = "Present";
        break;
    }
    return std::format_to(ctx.out(), "{}", name);
  }
};
