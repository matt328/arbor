#pragma once

#include <string>
#include <vulkan/vulkan_core.h>

#include "core/Device.hpp"
#include "bk/NonCopyMove.hpp"

namespace arb {

class PipelineLayout : NonCopyable {
public:
  explicit PipelineLayout(Device* newDevice,
                          const VkPipelineLayoutCreateInfo& createInfo,
                          const std::optional<std::string>& newName = std::nullopt);
  ~PipelineLayout();

  PipelineLayout(PipelineLayout&& other) noexcept;
  auto operator=(PipelineLayout&& other) noexcept -> PipelineLayout&;

  operator VkPipelineLayout() const {
    return pipelineLayout;
  }

private:
  Device* device;
  VkPipelineLayout pipelineLayout;
  std::string debugName;

  auto cleanup() -> void;
};

}
