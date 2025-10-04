#include "SamplerManager.hpp"

#include <cassert>

#include "bk/Logger.hpp"
#include "core/ImageHandle.hpp"

namespace arb {

SamplerManager::SamplerManager(Device& newDevice) : device{newDevice} {
  Log::trace("Creating SamplerManager");
}

SamplerManager::~SamplerManager() {
  Log::trace("Destroying SamplerManager");
}

auto SamplerManager::createSampler(const VkSamplerCreateInfo& createInfo,
                                   const std::optional<std::string>& name) -> SamplerHandle {
  const auto handle = handleGenerator.requestHandle();

  samplerMap.emplace(handle, std::make_unique<Sampler>(device, createInfo, name));

  return handle;
}

auto SamplerManager::destroySampler(SamplerHandle handle) -> void {
  assert(samplerMap.contains(handle));
  samplerMap.erase(handle);
}

auto SamplerManager::getSampler(SamplerHandle handle) -> Sampler& {
  assert(samplerMap.contains(handle));
  return *samplerMap.at(handle);
}

}
