#pragma once

#include <string>
#include <vulkan/vulkan_core.h>

#include "core/Device.hpp"
#include "bk/NonCopyMove.hpp"

namespace arb {

class Pipeline : public NonCopyable {
public:
  Pipeline(Device* newDevice,
           const VkGraphicsPipelineCreateInfo& createInfo,
           const std::optional<std::string>& newName = std::nullopt);

  Pipeline(Device* newDevice,
           const VkComputePipelineCreateInfo& createInfo,
           const std::optional<std::string>& newName = std::nullopt);

  Pipeline(Pipeline&&) noexcept;
  auto operator=(Pipeline&&) noexcept -> Pipeline&;

  ~Pipeline();

  operator VkPipeline() const {
    return pipeline;
  }

private:
  Device* device;
  VkPipeline pipeline;
  std::string debugName;

  auto cleanup() -> void;
};

}
