#include "Sampler.hpp"

#include "bk/Log.hpp"
#include "common/ErrorUtils.hpp"
#include "common/DebugUtils.hpp"
#include "vulkan/vulkan_core.h"

namespace arb {

Sampler::Sampler(Device& newDevice,
                 const VkSamplerCreateInfo& createInfo,
                 const std::optional<std::string>& name)
    : device{newDevice}, handle{VK_NULL_HANDLE}, debugName{name.value_or("Unnamed Sampler")} {
  LOG_TRACE_L1(Log::Resources, "Creating Sampler {}", debugName);
  checkVk(vkCreateSampler(device, &createInfo, nullptr, &handle), "vkCreateSampler");
  dbg::setDebugName(device, handle, debugName);
}

Sampler::~Sampler() {
  LOG_TRACE_L1(Log::Resources, "Destroying Sampler {}", debugName);
  if (handle != VK_NULL_HANDLE) {
    vkDestroySampler(device, handle, nullptr);
    handle = VK_NULL_HANDLE;
  }
}

}
