#pragma once

#include "framegraph/render-pass/pass/reqs/Enums.hpp"
#include "vulkan/vulkan_core.h"
#include <string>

namespace arb {

/// `LogicalImageRequirement` is how Dispatchers tell their passes what images they operate on and
/// how they'll operate on them. The Pass will map these LogicalImageRequirements to an image alias,
/// and register it with the AliasRegistry. AliasRegistry will materialize resources for aliases on
/// demand.
struct LogicalImageRequirement {
  std::string name;
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
