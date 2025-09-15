#pragma once

#include "framegraph/BufferUsageInfo.hpp"
#include "framegraph/ImageUsageInfo.hpp"

namespace arb {

struct PassGraphInfo {
  std::unordered_set<ImageUsageInfo> imageWrites{};
  std::unordered_set<ImageUsageInfo> imageReads{};

  std::unordered_set<BufferUsageInfo, BufferUsageInfoHash> bufferWrites{};
  std::unordered_set<BufferUsageInfo, BufferUsageInfoHash> bufferReads{};
};

}

template <typename Set, typename Context>
auto format_set(const Set& s, Context& ctx) {
  auto out = ctx.out();
  using T = typename Set::value_type;
  std::formatter<T> formatter;
  for (auto it = s.begin(); it != s.end(); ++it) {
    out = std::format_to(out, "\n    ");
    out = formatter.format(*it, ctx);
  }
  if (!s.empty()) {
    out = std::format_to(out, "\n  ");
  }
  return out;
}

template <typename Set, typename Context>
auto format_set2(const Set& s, Context& ctx) {
  auto out = ctx.out();
  bool first = true;
  for (const auto& elem : s) {
    if (!first) {
      out = std::format_to(out, ", ");
    }
    out = std::formatter<typename Set::value_type>{}.format(elem, ctx);
    first = false;
  }
  return out;
}

template <>
struct std::formatter<arb::PassGraphInfo> {
  constexpr auto parse(std::format_parse_context& ctx) {
    return ctx.begin();
  }

  auto format(const arb::PassGraphInfo& info, std::format_context& ctx) const {
    auto out = ctx.out();

    out = std::format_to(out, "PassGraphInfo {{\n  imageWrites: [");
    out = format_set(info.imageWrites, ctx);
    out = std::format_to(out, "],\n  imageReads: [");
    out = format_set(info.imageReads, ctx);
    out = std::format_to(out, "],\n  bufferWrites: [");
    out = format_set(info.bufferWrites, ctx);
    out = std::format_to(out, "],\n  bufferReads: [");
    out = format_set(info.bufferReads, ctx);
    out = std::format_to(out, "]\n}}");

    return out;
  }
};
