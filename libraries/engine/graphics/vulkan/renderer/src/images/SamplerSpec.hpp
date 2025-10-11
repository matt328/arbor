#pragma once

#include <optional>
#include <string>
#include <vulkan/vulkan_core.h>

namespace arb {

struct SamplerSpec {
  std::string logicalName; // Optional engine-level name for debug / lookup

  // Filtering
  VkFilter magFilter = VK_FILTER_LINEAR;
  VkFilter minFilter = VK_FILTER_LINEAR;
  VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

  // Addressing / wrap mode
  VkSamplerAddressMode addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  VkSamplerAddressMode addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  VkSamplerAddressMode addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

  // Level-of-detail
  float mipLodBias = 0.0f;
  float minLod = 0.0f;
  float maxLod = VK_LOD_CLAMP_NONE;

  // Optional features
  VkBorderColor borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  bool unnormalizedCoordinates = false;
  bool compareEnable = false;
  VkCompareOp compareOp = VK_COMPARE_OP_ALWAYS;

  // Anisotropy
  bool anisotropyEnable = false;
  float maxAnisotropy = 1.0f;

  std::optional<std::string> debugName = std::nullopt;

  auto operator==(const SamplerSpec& other) const -> bool {
    return magFilter == other.magFilter && minFilter == other.minFilter &&
           mipmapMode == other.mipmapMode && addressModeU == other.addressModeU &&
           addressModeV == other.addressModeV && addressModeW == other.addressModeW &&
           mipLodBias == other.mipLodBias && minLod == other.minLod && maxLod == other.maxLod &&
           borderColor == other.borderColor &&
           unnormalizedCoordinates == other.unnormalizedCoordinates &&
           compareEnable == other.compareEnable && compareOp == other.compareOp &&
           anisotropyEnable == other.anisotropyEnable && maxAnisotropy == other.maxAnisotropy &&
           debugName == other.debugName;
  }
};

}
