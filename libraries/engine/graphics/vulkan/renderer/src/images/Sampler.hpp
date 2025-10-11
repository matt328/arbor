#pragma once

#include "core/Device.hpp"

#include <string>
#include <vulkan/vulkan_core.h>

namespace arb {

class Sampler {
public:
  Sampler(Device& newDevice,
          const VkSamplerCreateInfo& createInfo,
          const std::optional<std::string>& name = std::nullopt);
  ~Sampler();

  operator VkSampler() const {
    return handle;
  }

private:
  Device& device;
  VkSampler handle;
  std::string debugName;
};

}
