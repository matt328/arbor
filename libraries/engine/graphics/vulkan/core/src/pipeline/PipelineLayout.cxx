#include "core/pipeline/PipelineLayout.hpp"

#include "common/ErrorUtils.hpp"
#include "common/DebugUtils.hpp"
#include "bk/Logger.hpp"

#include <vulkan/vulkan_core.h>

namespace arb {

PipelineLayout::PipelineLayout(Device* newDevice,
                               const VkPipelineLayoutCreateInfo& createInfo,
                               const std::optional<std::string>& newName)
    : device{newDevice},
      pipelineLayout{VK_NULL_HANDLE},
      debugName{newName.value_or("Unnamed PipelineLayout")} {
  Log::trace("Creating Pipeline Layout: {}", debugName);

  checkVk(vkCreatePipelineLayout(*device, &createInfo, nullptr, &pipelineLayout),
          "vkCreatePipelinelayout");
  dbg::setDebugName(*device, pipelineLayout, debugName);
}

PipelineLayout::~PipelineLayout() {
  Log::trace("Destroying PipelineLayout: {}", debugName);
  cleanup();
}

// Move constructor
PipelineLayout::PipelineLayout(PipelineLayout&& other) noexcept
    : device(other.device), pipelineLayout(std::exchange(other.pipelineLayout, VK_NULL_HANDLE)) {
}

// Move assignment
auto PipelineLayout::operator=(PipelineLayout&& other) noexcept -> PipelineLayout& {
  if (this != &other) {
    cleanup();
    device = other.device;
    pipelineLayout = std::exchange(other.pipelineLayout, VK_NULL_HANDLE);
  }
  return *this;
}

auto PipelineLayout::cleanup() -> void {
  if (pipelineLayout != VK_NULL_HANDLE) {
    vkDestroyPipelineLayout(*device, pipelineLayout, nullptr);
    pipelineLayout = VK_NULL_HANDLE;
  }
}

}
