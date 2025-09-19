#pragma once

#include <optional>
#include <string>
#include <vulkan/vulkan_core.h>

#include "core/Device.hpp"

namespace arb {

class ShaderModule {
public:
  explicit ShaderModule(Device* newDevice,
                        const VkShaderModuleCreateInfo& createInfo,
                        const std::optional<std::string>& newName = std::nullopt);
  ~ShaderModule();

  ShaderModule(ShaderModule&& other) noexcept;
  auto operator=(ShaderModule&& other) noexcept -> ShaderModule&;

  operator VkShaderModule() const {
    return handle;
  }

private:
  Device* device;
  VkShaderModule handle;
  std::string debugName;

  auto cleanup() -> void;
};

}
