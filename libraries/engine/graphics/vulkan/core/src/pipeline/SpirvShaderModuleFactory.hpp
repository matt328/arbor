#pragma once

#include "core/Device.hpp"
#include <filesystem>
#include "ShaderModule.hpp"

namespace arb {

class SpirvShaderModuleFactory {
public:
  [[nodiscard]] static auto createShaderModule(Device* device,
                                               VkShaderStageFlags shaderType,
                                               const std::filesystem::path& filename)
      -> ShaderModule;

private:
  [[nodiscard]] static auto readSPIRVFile(const std::string& filename) -> std::vector<uint32_t>;
};

}
