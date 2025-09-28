#include "SpirvShaderModuleFactory.hpp"

#include <cpptrace/cpptrace.hpp>
#include <fstream>

#include "bk/Logger.hpp"
#include "ShaderModule.hpp"
#include "vulkan/vulkan_core.h"

namespace arb {

auto SpirvShaderModuleFactory::createShaderModule(Device* device,
                                                  VkShaderStageFlags shaderType,
                                                  const std::filesystem::path& filename)
    -> ShaderModule {
  const auto spirv = readSPIRVFile(filename.string());
  const auto shaderCreateInfo =
      VkShaderModuleCreateInfo{.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                               .codeSize = 4 * spirv.size(),
                               .pCode = spirv.data()};
  return ShaderModule{device, shaderCreateInfo};
}

auto SpirvShaderModuleFactory::readSPIRVFile(const std::string& filename) -> std::vector<uint32_t> {
  std::ifstream file(filename, std::ios::binary | std::ios::ate);

  if (!file) {
    Log::error("Failed to open SPIR-V file: {}", filename);
    throw cpptrace::runtime_error("Failed to open SPIR-V file: " + filename);
  }

  // Get file size in bytes
  std::streamsize fileSize = file.tellg();

  if (fileSize % sizeof(uint32_t) != 0) {
    throw cpptrace::runtime_error("SPIR-V file size is not a multiple of 4 bytes");
  }

  std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

  // Seek back to the beginning and read into the vector
  file.seekg(0);
  file.read(reinterpret_cast<char*>(buffer.data()), fileSize);

  return buffer;
}

}
