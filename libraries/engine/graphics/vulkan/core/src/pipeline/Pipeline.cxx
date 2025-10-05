#include "core/pipeline/Pipeline.hpp"

#include "common/ErrorUtils.hpp"
#include "common/DebugUtils.hpp"

#include "bk/Log.hpp"
#include "vulkan/vulkan_core.h"

namespace arb {
Pipeline::Pipeline(Device* newDevice,
                   const VkGraphicsPipelineCreateInfo& createInfo,
                   const std::optional<std::string>& newName)
    : device{newDevice}, debugName{newName.value_or("Unnamed Pipeline")}, pipeline{VK_NULL_HANDLE} {
  LOG_TRACE_L1(Log::Core, "Creating Pipeline: {}", debugName);

  checkVk(vkCreateGraphicsPipelines(*device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline),
          "vkCreateGraphicsPipelines");

  dbg::setDebugName(*device, pipeline, debugName);
}

Pipeline::Pipeline(Device* newDevice,
                   const VkComputePipelineCreateInfo& createInfo,
                   const std::optional<std::string>& newName)
    : device{newDevice}, debugName{newName.value_or("Unnamed Pipeline")}, pipeline{VK_NULL_HANDLE} {
  checkVk(vkCreateComputePipelines(*device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline),
          "vkCeateComputePipelines");
  dbg::setDebugName(*device, pipeline, debugName);
}

Pipeline::~Pipeline() {
  LOG_TRACE_L1(Log::Core, "Destroying Pipeline: {}", debugName);
  cleanup();
}

Pipeline::Pipeline(Pipeline&& other) noexcept
    : device{other.device}, pipeline(std::exchange(other.pipeline, VK_NULL_HANDLE)) {
}

auto Pipeline::operator=(Pipeline&& other) noexcept -> Pipeline& {
  if (this != &other) {
    cleanup();
    device = other.device;
    pipeline = std::exchange(other.pipeline, VK_NULL_HANDLE);
  }
  return *this;
}

auto Pipeline::cleanup() -> void {
  if (pipeline != VK_NULL_HANDLE) {
    vkDestroyPipeline(*device, pipeline, nullptr);
    pipeline = VK_NULL_HANDLE;
  }
}

}
