#pragma once

#include "common/Handles.hpp"
#include "core/Device.hpp"
#include "engine/common/HandleGenerator.hpp"
#include "core/ImageHandle.hpp"
#include "images/Sampler.hpp"

#include <string>

namespace arb {

class SamplerManager {
public:
  explicit SamplerManager(Device& newDevice);
  ~SamplerManager();

  auto createSampler(const VkSamplerCreateInfo& createInfo, const std::optional<std::string>& name)
      -> SamplerHandle;
  auto destroySampler(SamplerHandle handle) -> void;
  auto getSampler(SamplerHandle handle) -> Sampler&;

private:
  Device& device;
  HandleGenerator<SamplerTag> handleGenerator;
  std::unordered_map<SamplerHandle, std::unique_ptr<Sampler>> samplerMap;
};

}
