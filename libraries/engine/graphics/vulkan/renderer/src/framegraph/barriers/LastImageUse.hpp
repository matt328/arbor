#pragma once

#include "AccessMode.hpp"
#include <vulkan/vulkan_core.h>

namespace arb {

struct LastImageUse {
  AccessMode accessMode{};
  VkAccessFlags2 access{};
  VkPipelineStageFlags2 stage{};
  VkImageLayout layout{};
};

}
