#pragma once

#include <filesystem>
#include <vulkan/vulkan_core.h>

#include "bk/NonCopyMove.hpp"
#include "ShaderModule.hpp"

namespace arb {

class IShaderModuleFactory : NonCopyableMovable {
public:
  IShaderModuleFactory() = default;
  virtual ~IShaderModuleFactory() = default;

  [[nodiscard]] virtual auto createShaderModule(VkShaderStageFlags shaderType,
                                                const std::filesystem::path& filename) const
      -> std::unique_ptr<ShaderModule> = 0;
};

}
