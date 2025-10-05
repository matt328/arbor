#include "core/pipeline/PipelineManager.hpp"

#include "SpirvShaderModuleFactory.hpp"

#include "bk/Log.hpp"
#include "engine/common/HandleGenerator.hpp"

#include <array>
#include <cassert>
#include <vulkan/vulkan_core.h>

namespace arb {

PipelineManager::PipelineManager(Device& newDevice) : device{newDevice} {
  LOG_TRACE_L1(Log::Core, "Creating PipelineManager");
}

PipelineManager::~PipelineManager() {
  LOG_TRACE_L1(Log::Core, "Destroying PipelineManager");
  pipelineMap.clear();
}

[[nodiscard]] auto PipelineManager::getPipelineUnit(PipelineUnitHandle handle) const
    -> const PipelineUnit& {
  assert(pipelineMap.contains(handle));
  return *pipelineMap.at(handle);
}

auto PipelineManager::createPipeline(const PipelineCreateInfo& createInfo) -> PipelineUnitHandle {
  const auto handle = handleGenerator.requestHandle();
  if (createInfo.pipelineType == PipelineType::Graphics) {
    pipelineMap.emplace(handle, createGraphicsPipeline(createInfo));
  } else {
    pipelineMap.emplace(handle, createComputePipeline(createInfo));
  }
  return handle;
}

auto PipelineManager::createGraphicsPipeline(const PipelineCreateInfo& createInfo)
    -> std::unique_ptr<PipelineUnit> {
  const auto& layoutInfo = createInfo.pipelineLayoutInfo;

  // Push Constants
  auto pushConstantRanges = std::vector<VkPushConstantRange>{};
  for (const auto& pcrInfo : layoutInfo.pushConstantInfoList) {
    pushConstantRanges.emplace_back(VkPushConstantRange{
        .stageFlags = pcrInfo.stageFlags,
        .offset = pcrInfo.offset,
        .size = pcrInfo.size,
    });
  }

  // DescriptorSetLayouts
  const auto layoutCreateInfo = VkPipelineLayoutCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .setLayoutCount = 0,
      .pSetLayouts = nullptr,
      .pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size()),
      .pPushConstantRanges = pushConstantRanges.data()};

  auto pipelineLayout = PipelineLayout{&device, layoutCreateInfo};

  // RenderingInfo
  auto pipelineRenderingInfo = VkPipelineRenderingCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
      .colorAttachmentCount = static_cast<uint32_t>(createInfo.colorAttachmentFormats.size()),
      .pColorAttachmentFormats = createInfo.colorAttachmentFormats.data()};
  if (createInfo.depthAttachmentFormat) {
    pipelineRenderingInfo.depthAttachmentFormat = *createInfo.depthAttachmentFormat;
  }

  // Shader Stages
  auto shaderModules = std::vector<ShaderModule>{};
  auto shaderStages = std::vector<VkPipelineShaderStageCreateInfo>{};
  for (const auto& stageInfo : createInfo.shaderStageInfo) {
    shaderModules.emplace_back(SpirvShaderModuleFactory::createShaderModule(&device,
                                                                            stageInfo.stage,
                                                                            stageInfo.shaderFile));
    shaderStages.emplace_back(VkPipelineShaderStageCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = stageInfo.stage,
        .module = shaderModules.back(),
        .pName = stageInfo.entryPoint.c_str()});
  }

  // VertexInputState
  const auto vertexInputStateCreateInfo = VkPipelineVertexInputStateCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
      .vertexBindingDescriptionCount = 0,
      .pVertexBindingDescriptions = nullptr,
      .vertexAttributeDescriptionCount = 0,
      .pVertexAttributeDescriptions = nullptr};

  // Input Assembly
  const auto inputAssembly = VkPipelineInputAssemblyStateCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .topology = createInfo.topology,
      .primitiveRestartEnable = VK_FALSE,
  };

  // Rasterizer
  const auto rasterizer = VkPipelineRasterizationStateCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
      .polygonMode = createInfo.polygonMode,
      .cullMode = createInfo.cullMode,
      .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
      .lineWidth = 1.f,
  };

  // Multisampling
  constexpr auto multisampling = VkPipelineMultisampleStateCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
      .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
      .sampleShadingEnable = VK_FALSE,
      .minSampleShading = 1.f};

  // Depth Stencil
  const auto depthStencil = VkPipelineDepthStencilStateCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
      .depthTestEnable = static_cast<VkBool32>(createInfo.enableDepthTest),
      .depthWriteEnable = static_cast<VkBool32>(createInfo.enableDepthWrite),
      .depthCompareOp = VK_COMPARE_OP_LESS,
      .depthBoundsTestEnable = VK_FALSE,
      .stencilTestEnable = VK_FALSE};

  // Color Blending
  constexpr auto colorBlendAttachment = VkPipelineColorBlendAttachmentState{
      .blendEnable = VK_FALSE,
      .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT};
  const auto colorBlending = VkPipelineColorBlendStateCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
      .logicOpEnable = VK_FALSE,
      .logicOp = VK_LOGIC_OP_COPY,
      .attachmentCount = 1,
      .pAttachments = &colorBlendAttachment};

  // Dynamic State
  constexpr auto dynamicStates = std::array<VkDynamicState, 2>{
      VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT,
      VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT,
  };

  const auto dynamicStateInfo = VkPipelineDynamicStateCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
      .dynamicStateCount = 2,
      .pDynamicStates = dynamicStates.data()};

  // Pipeline Itself
  const auto pipelineCreateInfo =
      VkGraphicsPipelineCreateInfo{.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                                   .pNext = &pipelineRenderingInfo,
                                   .stageCount = static_cast<uint32_t>(shaderStages.size()),
                                   .pStages = shaderStages.data(),
                                   .pVertexInputState = &vertexInputStateCreateInfo,
                                   .pInputAssemblyState = &inputAssembly,
                                   .pRasterizationState = &rasterizer,
                                   .pMultisampleState = &multisampling,
                                   .pDepthStencilState = &depthStencil,
                                   .pColorBlendState = &colorBlending,
                                   .pDynamicState = &dynamicStateInfo,
                                   .layout = pipelineLayout,
                                   .subpass = 0,
                                   .basePipelineHandle = VK_NULL_HANDLE,
                                   .basePipelineIndex = -1};

  auto pipeline = Pipeline{&device, pipelineCreateInfo};
  return std::make_unique<PipelineUnit>(std::move(pipeline), std::move(pipelineLayout));
}

auto PipelineManager::createComputePipeline(const PipelineCreateInfo& createInfo)
    -> std::unique_ptr<PipelineUnit> {
  std::vector<VkPushConstantRange> pushConstantRanges{
      createInfo.pipelineLayoutInfo.pushConstantInfoList.size()};

  for (const auto& pcrInfo : createInfo.pipelineLayoutInfo.pushConstantInfoList) {
    pushConstantRanges.emplace_back(VkPushConstantRange{
        .stageFlags = pcrInfo.stageFlags,
        .offset = pcrInfo.offset,
        .size = pcrInfo.size,
    });
  }

  const auto layoutCreateInfo = VkPipelineLayoutCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size()),
      .pPushConstantRanges = pushConstantRanges.data()};

  auto pipelineLayout = PipelineLayout{&device, layoutCreateInfo};

  // Shader Stages
  auto shaderModules = std::vector<ShaderModule>{};
  auto shaderStages = std::vector<VkPipelineShaderStageCreateInfo>{};
  for (const auto& stageInfo : createInfo.shaderStageInfo) {
    shaderModules.emplace_back(SpirvShaderModuleFactory::createShaderModule(&device,
                                                                            stageInfo.stage,
                                                                            stageInfo.shaderFile));
    shaderStages.emplace_back(VkPipelineShaderStageCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = stageInfo.stage,
        .module = shaderModules.back(),
        .pName = stageInfo.entryPoint.c_str()});
  }

  const auto pipelineCreateInfo =
      VkComputePipelineCreateInfo{.stage = shaderStages.front(), .layout = pipelineLayout};

  auto pipeline = Pipeline{&device, pipelineCreateInfo};
  return std::make_unique<PipelineUnit>(std::move(pipeline), std::move(pipelineLayout));
}

}
