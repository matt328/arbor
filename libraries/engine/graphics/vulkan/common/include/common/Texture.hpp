#pragma once

#include <vulkan/vulkan_core.h>

namespace arb {

/// Placeholder struct wrapping an ImageView and a Sampler
struct Texture {
  VkImageView view;
  VkSampler sampler;
};

}
