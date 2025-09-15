#pragma once

#include "framegraph/ComponentIds.hpp"
#include "vulkan/vulkan_core.h"

namespace arb {

struct LastBufferUse {
  PassId passId;
  VkAccessFlags2 accessMask;
  VkPipelineStageFlags2 stageMask;
};

}
