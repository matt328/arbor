#pragma once

#include <vulkan/vulkan_core.h>

namespace arb {

struct Image {
  VkImage vkImage;

  [[nodiscard]] operator VkImage() const {
    return vkImage;
  }
};

}
