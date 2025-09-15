#pragma once

#include <unordered_map>

#include "framegraph/ComponentIds.hpp"
#include "framegraph/barriers/BufferBarrierPrecursor.hpp"
#include "framegraph/barriers/ImageBarrierPrecursor.hpp"

namespace arb {

struct BarrierPrecursorPlan {
  std::unordered_map<PassId, std::vector<ImageBarrierPrecursor>> imagePrecursors;
  std::unordered_map<PassId, std::vector<BufferBarrierPrecursor>> bufferPrecursors;
};

}

template <>
struct std::formatter<arb::BarrierPrecursorPlan> {
  // NOLINTNEXTLINE
  constexpr auto parse(std::format_parse_context& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const arb::BarrierPrecursorPlan& plan, FormatContext& ctx) const {
    auto out = ctx.out();
    out = std::format_to(out, "BarrierPrecursorPlan {{\n");

    out = std::format_to(out, "  ImagePrecursors:\n");
    for (const auto& [passId, precursors] : plan.imagePrecursors) {
      out = std::format_to(out, "    [{}]:\n", passId);
      for (const auto& precursor : precursors) {
        out = std::format_to(out, "      {}\n", precursor);
      }
    }

    out = std::format_to(out, "  BufferPrecursors:\n");
    for (const auto& [passId, precursors] : plan.bufferPrecursors) {
      out = std::format_to(out, "    [{}]:\n", passId);
      for (const auto& precursor : precursors) {
        out = std::format_to(out, "      {}\n", precursor);
      }
    }

    return std::format_to(out, "}}\n");
  }
};
