#pragma once

#include <string>
#include <vulkan/vulkan_core.h>

#include "Enums.hpp"

namespace arb {

/// `ImageRequirement` is how Image Aliases and their backing resources are requested from the
/// `AliasRegistry`. `IRenderPass`, or `FrameRenderer` itself will register these with the
/// `AliasRegistry`. The `AliasRegistry` will deduplicate requirements and create the vulkan
/// resources that back them. At command buffer recording time, passes will request the vulkan
/// resources backing them.
struct ImageRequirement {
  /// The alias
  std::string alias;
  /// Global means all frames use the same image. Non-Global means create a per-frame image and
  /// request it with a frameIndex
  bool isGlobal{false};
  /// What does the pass do with this image, Read, Write, or Both
  AliasAccess accessType;
  /// How does the pass use this image
  AliasUseType usageType;
  VkFormat format;
  VkExtent2D extent;
  VkImageUsageFlags usage;
};

}
