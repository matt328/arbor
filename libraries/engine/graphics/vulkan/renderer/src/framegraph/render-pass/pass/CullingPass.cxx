#include "CullingPass.hpp"

#include <filesystem>

#include "bk/Log.hpp"
#include "common/BufferUseDescription.hpp"
#include "reqs/PassDescription.hpp"
#include "ResourceAliases.hpp"
#include "common/BufferCreateInfo.hpp"
#include "vulkan/vulkan_core.h"

namespace arb {

CullingPass::CullingPass(const CullingPassDeps& deps, const CullingPassConfig& config)
    : aliasRegistry{deps.aliasRegistry},
      pipelineManager{deps.pipelineManager},
      resourceTable{config.resourceTable},
      frameData{config.frameData},
      surfaceState{config.initialState} {
  LOG_TRACE_L1(Log::Renderer, "Creating CullingPass");

  const auto pipelineLayoutInfo = PipelineLayoutInfo{.pushConstantInfoList = {PushConstantInfo{
                                                         .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
                                                         .offset = 0,
                                                         .size = 104, // TODO(matt) sizeof()
                                                     }}};

  const auto shaderStageInfo = ShaderStageInfo{
      .stage = VK_SHADER_STAGE_COMPUTE_BIT,
      .shaderFile =
          (std::filesystem::current_path() / "assets" / "shaders" / "compute2.comp.spv").string(),
      .entryPoint = "main"};

  const auto pipelineCreateInfo = PipelineCreateInfo{.pipelineType = PipelineType::Compute,
                                                     .pipelineLayoutInfo = pipelineLayoutInfo,
                                                     .shaderStageInfo = {shaderStageInfo},
                                                     .debugName = "CullingPipeline"};

  pipelineHandle = deps.pipelineManager.createPipeline(pipelineCreateInfo);
}

CullingPass::~CullingPass() {
  LOG_TRACE_L1(Log::Renderer, "Destroying CullingPass");
}

auto CullingPass::getId() const -> PassId {
  return PassId::Culling;
}

void CullingPass::resize(const RenderSurfaceState& newState) {
  surfaceState = newState;
}

void CullingPass::execute(Frame* frame, VkCommandBuffer cmdBuffer) {
}

auto CullingPass::getDescription() const -> PassDescription {

  const auto buffers = std::vector<BufferRequirement>{
      BufferRequirement{
          .alias = BufferAlias::IndirectCommand,
          .useDesc = BufferUseDescription{.alias = BufferAlias::IndirectCommand,
                                          .accessFlags = VK_ACCESS_2_SHADER_WRITE_BIT,
                                          .stageFlags = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT},
          .createDesc = BufferCreateInfo{.bufferLifetime = BufferLifetime::Transient,
                                         .initialSize = 20480,
                                         .debugName = "IndirectCommand",
                                         .indirect = true},
      },
      BufferRequirement{
          .alias = BufferAlias::IndirectCommandCount,
          .useDesc = BufferUseDescription{.alias = BufferAlias::IndirectCommandCount,
                                          .accessFlags = VK_ACCESS_2_SHADER_WRITE_BIT,
                                          .stageFlags = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT},
          .createDesc = BufferCreateInfo{.bufferLifetime = BufferLifetime::Transient,
                                         .initialSize = 4,
                                         .debugName = "IndirectCommandCount",
                                         .indirect = true},
      },
      BufferRequirement{.alias = BufferAlias::ObjectData,
                        .useDesc = {.alias = BufferAlias::ObjectData,
                                    .accessFlags = VK_ACCESS_2_SHADER_READ_BIT,
                                    .stageFlags = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT}},
      BufferRequirement{.alias = BufferAlias::ObjectPositions,
                        .useDesc = {.alias = BufferAlias::ObjectPositions,
                                    .accessFlags = VK_ACCESS_2_SHADER_READ_BIT,
                                    .stageFlags = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT}},
      BufferRequirement{.alias = BufferAlias::ObjectRotations,
                        .useDesc = {.alias = BufferAlias::ObjectRotations,
                                    .accessFlags = VK_ACCESS_2_SHADER_READ_BIT,
                                    .stageFlags = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT}},
      BufferRequirement{.alias = BufferAlias::ObjectScales,
                        .useDesc = {.alias = BufferAlias::ObjectScales,
                                    .accessFlags = VK_ACCESS_2_SHADER_READ_BIT,
                                    .stageFlags = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT}},
      BufferRequirement{.alias = BufferAlias::GeometryRegion,
                        .useDesc = {.alias = BufferAlias::GeometryRegion,
                                    .accessFlags = VK_ACCESS_2_SHADER_READ_BIT,
                                    .stageFlags = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT}},
      BufferRequirement{.alias = GlobalBufferAlias::Index,
                        .useDesc = {.alias = GlobalBufferAlias::Index,
                                    .accessFlags = VK_ACCESS_2_SHADER_READ_BIT,
                                    .stageFlags = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT}},
      BufferRequirement{.alias = GlobalBufferAlias::Normal,
                        .useDesc = {.alias = GlobalBufferAlias::Normal,
                                    .accessFlags = VK_ACCESS_2_SHADER_READ_BIT,
                                    .stageFlags = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT}},
      BufferRequirement{.alias = GlobalBufferAlias::Normal,
                        .useDesc = {.alias = GlobalBufferAlias::Normal,
                                    .accessFlags = VK_ACCESS_2_SHADER_READ_BIT,
                                    .stageFlags = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT}},
      BufferRequirement{.alias = GlobalBufferAlias::TexCoord,
                        .useDesc = {.alias = GlobalBufferAlias::TexCoord,
                                    .accessFlags = VK_ACCESS_2_SHADER_READ_BIT,
                                    .stageFlags = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT}}};

  return PassDescription{
      .name = "CullingPass",
      .images = {},
      .buffers = buffers,
      .dependsOn = {},
  };
}
}
