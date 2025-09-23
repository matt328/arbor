#include "resources/images/Sampler.hpp"

#include "bk/Logger.hpp"
#include "common/ErrorUtils.hpp"
#include "common/DebugUtils.hpp"
#include "vulkan/vulkan_core.h"

namespace arb {

Sampler::Sampler(Device& newDevice,
                 const VkSamplerCreateInfo& createInfo,
                 const std::optional<std::string>& name)
    : device{newDevice}, handle{VK_NULL_HANDLE}, debugName{name.value_or("Unnamed Sampler")} {
  Log::trace("Creating Sampler {}", debugName);
  checkVk(vkCreateSampler(device, &createInfo, nullptr, &handle), "vkCreateSampler");
  dbg::setDebugName(device, handle, debugName);
}

Sampler::~Sampler() {
  Log::trace("Destroying Sampler {}", debugName);
  if (handle != VK_NULL_HANDLE) {
    vkDestroySampler(device, handle, nullptr);
    handle = VK_NULL_HANDLE;
  }
}

}
