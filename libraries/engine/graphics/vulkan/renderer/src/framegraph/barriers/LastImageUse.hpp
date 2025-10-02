#pragma once

#include <vulkan/vulkan_core.h>

namespace arb {

struct LastImageUse {
  bool isWriteAccess{false};
  VkAccessFlags2 access{};
  VkPipelineStageFlags2 stage{};
  VkImageLayout layout{};
};

}
