#pragma once

#include "BufferCreateInfo.hpp"
#include "BufferUseDescription.hpp"

#include <optional>
#include <string>
#include <vulkan/vulkan_core.h>

namespace arb {

struct BufferRequirement {
  std::string alias;
  BufferUseDescription useDesc;
  std::optional<BufferCreateInfo> createDesc = std::nullopt;
};

}
