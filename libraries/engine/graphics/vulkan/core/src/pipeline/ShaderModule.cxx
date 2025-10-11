#include "ShaderModule.hpp"

#include "bk/Log.hpp"
#include "common/ErrorUtils.hpp"
#include "common/DebugUtils.hpp"
#include "vulkan/vulkan_core.h"

namespace arb {

ShaderModule::ShaderModule(Device* newDevice,
                           const VkShaderModuleCreateInfo& createInfo,
                           const std::optional<std::string>& newName)
    : device{newDevice},
      handle{VK_NULL_HANDLE},
      debugName{newName.value_or("Unnamed ShaderModule")} {
  checkVk(vkCreateShaderModule(*device, &createInfo, nullptr, &handle), "vkCeateShaderModule");
  dbg::setDebugName(*device, handle, debugName);
}

ShaderModule::~ShaderModule() {
  if (handle != VK_NULL_HANDLE) {
    LOG_TRACE_L1(Log::Core, "Destroying ShaderModule: {}", debugName);
    cleanup();
  }
}

ShaderModule::ShaderModule(ShaderModule&& other) noexcept
    : device{other.device},
      handle{std::exchange(other.handle, VK_NULL_HANDLE)},
      debugName{std::move(other.debugName)} {
}

auto ShaderModule::operator=(ShaderModule&& other) noexcept -> ShaderModule& {
  if (this != &other) {
    cleanup();
    device = other.device;
    handle = std::exchange(other.handle, VK_NULL_HANDLE);
    debugName = std::move(other.debugName);
  }
  return *this;
}

auto ShaderModule::cleanup() -> void {
  if (handle != VK_NULL_HANDLE) {
    vkDestroyShaderModule(*device, handle, nullptr);
  }
}

}
