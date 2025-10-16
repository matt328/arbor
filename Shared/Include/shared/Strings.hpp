#pragma once

#include <cstdint>
#include <regex>
#include <string>

namespace Arboror::Strings {

static inline auto incrementVersion(const std::string& name) -> std::string {
  static const std::regex versionRegex(R"(^(.*-v)(\d+)$)");
  std::smatch match;
  if (std::regex_match(name, match, versionRegex)) {
    uint32_t v = std::stoi(match[2].str());
    return match[1].str() + std::to_string(v + 1);
  }
  return name + "-v1";
}

}
