#pragma once

#include "common/ImageUseDescription.hpp"
#include <string>

namespace arb {

struct ExternalImageRequirement {
  std::string alias;
  ImageUseDescription useDesc;
};

}

template <>
struct std::formatter<arb::ExternalImageRequirement> : std::formatter<std::string> {
  auto format(const arb::ExternalImageRequirement& req, std::format_context& ctx) const {
    return std::formatter<std::string>::format(
        std::format("ImageRequirement(alias=\"{}\", useDesc={})", req.alias, req.useDesc),
        ctx);
  }
};
