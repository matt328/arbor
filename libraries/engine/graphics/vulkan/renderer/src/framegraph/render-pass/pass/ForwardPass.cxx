#include "ForwardPass.hpp"

#include "Frame.hpp"
#include "bk/Logger.hpp"
#include "core/pipeline/PipelineUnitCreateInfo.hpp"
#include "framegraph/render-pass/dispatcher/IDispatcher.hpp"
#include "reqs/Enums.hpp"
#include "reqs/ImageRequest.hpp"
#include "framegraph/render-pass/dispatcher/ClearDispatcher.hpp"

#include <filesystem>
#include <Tracy.hpp>
#include <vulkan/vulkan_core.h>

namespace arb {

ForwardPass::ForwardPass(const ForwardPassConfig& config)
    : aliasRegistry{config.aliasRegistry},
      pipelineManager{config.pipelineManager},
      pipelineHandle{config.pipelineHandle} {
  Log::trace("Creating ForwardPass");

  auto clear = std::make_unique<ClearDispatcher>(aliasRegistry);

  bindingMap.emplace("ColorTarget", "SwapchainImage");
  bindingMap.emplace("DepthTarget", "DepthBuffer");

  dispatchers.push_back(std::move(clear));
}

ForwardPass::~ForwardPass() {
  Log::trace("Destroying ForwardPass");
}

auto ForwardPass::create(const ForwardPassContext& ctx) -> std::unique_ptr<ForwardPass> {
  const auto pipelineLayoutInfo = PipelineLayoutInfo{
      .pushConstantInfoList = {
          PushConstantInfo{.stageFlags = VK_SHADER_STAGE_VERTEX_BIT, .offset = 0, .size = 36}}};

  const auto vertexStage = ShaderStageInfo{
      .stage = VK_SHADER_STAGE_VERTEX_BIT,
      .shaderFile =
          (std::filesystem::current_path() / "assets" / "shaders" / "composition.vert.spv")
              .string(),
      .entryPoint = "main",
  };

  const auto fragmentStage = ShaderStageInfo{
      .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
      .shaderFile =
          (std::filesystem::current_path() / "assets" / "shaders" / "composition.frag.spv")
              .string(),
      .entryPoint = "main",
  };

  const auto pipelineCreateInfo =
      PipelineCreateInfo{.pipelineType = PipelineType::Graphics,
                         .pipelineLayoutInfo = pipelineLayoutInfo,
                         .colorAttachmentFormats =
                             {VK_FORMAT_R8G8B8A8_UNORM}, // Get swapchain attachment format somehow
                         .shaderStageInfo = {vertexStage, fragmentStage}};

  auto handle = ctx.pipelineManager.createPipeline(pipelineCreateInfo);

  const auto config = ForwardPassConfig{
      .aliasRegistry = ctx.aliasRegistry,
      .pipelineManager = ctx.pipelineManager,
      .pipelineHandle = handle,
  };

  return std::unique_ptr<ForwardPass>(new ForwardPass(config));
}

auto ForwardPass::getDescription() const -> PassDescription {
  auto desc = PassDescription{};
  desc.name = "ForwardPass";
  // Pass Resources
  desc.images = {ImageRequirement{.alias = "SwapchainImage",
                                  .isGlobal = false,
                                  .accessType = AliasAccess::Write,
                                  .usageType = AliasUseType::Attachment,
                                  .format = VK_FORMAT_R8G8B8A8_UNORM,
                                  .extent = {},
                                  .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT},
                 ImageRequirement{.alias = "DepthBuffer",
                                  .isGlobal = false,
                                  .accessType = AliasAccess::Write,
                                  .usageType = AliasUseType::Attachment,
                                  .format = VK_FORMAT_D32_SFLOAT,
                                  .extent = {},
                                  .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT}};
  desc.buffers = {};
  desc.dependsOn = {};

  // Add dispatcher Resources
  for (const auto& dispatcher : dispatchers) {
    const auto reqs = dispatcher->requirements();
    for (const auto& lir : reqs.images) {
      auto ir = ImageRequirement{};
      ir.alias = bindingMap.at(lir.name);
      ir.isGlobal = lir.isGlobal;
      ir.accessType = lir.accessType;
      ir.usageType = lir.usageType;
      ir.format = lir.format;
      ir.extent = lir.extent;
      ir.usage = lir.usage;
      desc.images.push_back(ir);
    }
  }

  return desc;
}

[[nodiscard]] auto ForwardPass::getId() const -> PassId {
  return PassId::Forward;
}

auto ForwardPass::execute(Frame* frame, VkCommandBuffer cmdBuffer) -> void {
  ZoneScoped;
  const auto colorAttachmentInfo = aliasRegistry.getAttachmentInfo(
      "forward.color",
      frame->getIndex(),
      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      VK_ATTACHMENT_LOAD_OP_CLEAR,
      VK_ATTACHMENT_STORE_OP_STORE,
      VkClearValue{.color = {.float32 = {0.392f, 0.584f, 0.929f, 1.0f}}});

  const auto depthAttachmentInfo = aliasRegistry.getAttachmentInfo(
      "forward.depth",
      frame->getIndex(),
      VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
      VK_ATTACHMENT_LOAD_OP_CLEAR,
      VK_ATTACHMENT_STORE_OP_STORE,
      VkClearValue{.depthStencil = VkClearDepthStencilValue{.depth = 1.f, .stencil = 0}});

  const auto renderingInfo = VkRenderingInfo{
      .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
      .renderArea =
          (VkRect2D{.offset = {.x = 0, .y = 0}, .extent = {.width = 1920, .height = 1080}}),
      .colorAttachmentCount = 1,
      .pColorAttachments = &colorAttachmentInfo,
      .pDepthAttachment = &depthAttachmentInfo};

  if (!frame->getImageTransitionInfo().empty()) {
    auto barriers = std::vector<VkImageMemoryBarrier2>{};
    for (const auto& info : frame->getImageTransitionInfo()) {
      barriers.push_back({.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
                          .srcAccessMask = VK_ACCESS_2_NONE,
                          .dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
                          .dstAccessMask = VK_ACCESS_2_NONE,
                          .oldLayout = info.oldLayout,
                          .newLayout = info.newLayout,
                          .image = info.image,
                          .subresourceRange = info.subresourceRange});
    }

    if (!barriers.empty()) {
      auto dependencyInfo =
          VkDependencyInfo{.imageMemoryBarrierCount = static_cast<uint32_t>(barriers.size()),
                           .pImageMemoryBarriers = barriers.data()};
      vkCmdPipelineBarrier2(cmdBuffer, &dependencyInfo);
    }
  }

  const auto& pipelineUnit = pipelineManager.getPipelineUnit(pipelineHandle);

  vkCmdBeginRendering(cmdBuffer, &renderingInfo);
  vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineUnit.pipeline);

  for (const auto& dispatcher : dispatchers) {
    dispatcher->dispatch(frame, pipelineUnit.layout, cmdBuffer);
  }

  vkCmdEndRendering(cmdBuffer);
}

}
