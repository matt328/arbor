#include "CullingPass.hpp"

#include <filesystem>

#include "bk/Log.hpp"
#include "reqs/PassDescription.hpp"
#include "vulkan/vulkan_core.h"

namespace arb {

CullingPass::CullingPass(const CullingPassDeps& deps, const CullingPassConfig& config)
    : aliasRegistry{deps.aliasRegistry},
      pipelineManager{deps.pipelineManager},
      resourceTable{config.resourceTable},
      frameData{config.frameData} {
  LOG_TRACE_L1(Log::Renderer, "Creating CullingPass");

  const auto pipelineLayoutInfo = PipelineLayoutInfo{.pushConstantInfoList = {PushConstantInfo{
                                                         .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
                                                         .offset = 0,
                                                         .size = 104, // TODO(matt) sizeof()
                                                     }}};

  const auto shaderStageInfo = ShaderStageInfo{
      .stage = VK_SHADER_STAGE_COMPUTE_BIT,
      .shaderFile = (std::filesystem::current_path() / "compute2.comp.spv").string(),
      .entryPoint = "main"};

  const auto pipelineCreateInfo = PipelineCreateInfo{.pipelineType = PipelineType::Compute,
                                                     .pipelineLayoutInfo = pipelineLayoutInfo,
                                                     .shaderStageInfo = {shaderStageInfo}};

  pipelineHandle = deps.pipelineManager.createPipeline(pipelineCreateInfo);
}

CullingPass::~CullingPass() {
  LOG_TRACE_L1(Log::Renderer, "Destroying CullingPass");
}

auto CullingPass::getId() const -> PassId {
  return PassId::Culling;
}

void CullingPass::execute(Frame* frame, VkCommandBuffer cmdBuffer) {
}

auto CullingPass::getDescription() const -> PassDescription {
  const auto desc = PassDescription{
      .name = "CullingPass",
      .images = {},
      .buffers = {BufferRequest2{.}},
      .dependsOn = {},
  };

  return desc;
}

}
