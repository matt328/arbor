#pragma once

#include "vulkan/vulkan_core.h"

namespace arb {

struct ExternalImageDescription {
  VkImage vkImage;
  VkExtent2D extent;
  VkFormat format;
};

}
