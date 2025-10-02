#include "ForwardPass.hpp"

#include "Frame.hpp"
#include "bk/Logger.hpp"
#include "common/ImageCreateDescription.hpp"
#include "common/ImageLifetime.hpp"
#include "core/pipeline/PipelineUnitCreateInfo.hpp"
#include "common/ImageRequirement.hpp"

#include <cpptrace/cpptrace.hpp>
#include <cpptrace/exceptions.hpp>
#include <filesystem>
#include <Tracy.hpp>
#include <vulkan/vulkan_core.h>

namespace arb {

ForwardPass::ForwardPass(const ForwardPassDeps& deps, const ForwardPassConfig& config)
    : aliasRegistry{deps.aliasRegistry},
      pipelineManager{deps.pipelineManager},
      surfaceState{config.initialSurfaceState} {
  Log::trace("Creating ForwardPass");

  const auto pipelineLayoutInfo = PipelineLayoutInfo{
      .pushConstantInfoList = {
          PushConstantInfo{.stageFlags = VK_SHADER_STAGE_VERTEX_BIT, .offset = 0, .size = 36}}};

  const auto vertexStage = ShaderStageInfo{
      .stage = VK_SHADER_STAGE_VERTEX_BIT,
      .shaderFile =
          (std::filesystem::current_path() / "assets" / "shaders" / "forward2.vert.spv").string(),
      .entryPoint = "main",
  };

  const auto fragmentStage = ShaderStageInfo{
      .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
      .shaderFile =
          (std::filesystem::current_path() / "assets" / "shaders" / "forward2.frag.spv").string(),
      .entryPoint = "main",
  };

  const auto pipelineCreateInfo =
      PipelineCreateInfo{.pipelineType = PipelineType::Graphics,
                         .pipelineLayoutInfo = pipelineLayoutInfo,
                         .colorAttachmentFormats =
                             {VK_FORMAT_R8G8B8A8_UNORM}, // Get swapchain attachment format somehow
                         .shaderStageInfo = {vertexStage, fragmentStage}};

  pipelineHandle = deps.pipelineManager.createPipeline(pipelineCreateInfo);
}

ForwardPass::~ForwardPass() {
  Log::trace("Destroying ForwardPass");
}

auto ForwardPass::getDescription() const -> PassDescription {
  auto desc = PassDescription{};
  desc.name = "ForwardPass";
  // Pass Resources
  desc.images = {
      ImageRequirement{.alias = "SwapchainImage",
                       .createDesc = std::make_optional<ImageCreateDescription>(
                           {.format = VK_FORMAT_R8G8B8A8_UNORM,
                            .extent = {.width = surfaceState.swapchainExtent.width,
                                       .height = surfaceState.swapchainExtent.height},
                            .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                            .imageLifetime = ImageLifetime::Swapchain}),
                       .useDesc = {.accessFlags = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                                   .stageFlags = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                                   .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
                                   .aspectFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT}},
      ImageRequirement{.alias = "DepthBuffer",
                       .createDesc = std::make_optional<ImageCreateDescription>(
                           {.format = VK_FORMAT_D32_SFLOAT,
                            .extent = {.width = surfaceState.swapchainExtent.width,
                                       .height = surfaceState.swapchainExtent.height},
                            .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                            .imageLifetime = ImageLifetime::Persistent}),
                       .useDesc = {.accessFlags = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                                   .stageFlags = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT |
                                                 VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
                                   .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
                                   .aspectFlags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT}}};
  desc.buffers = {};
  desc.dependsOn = {};

  return desc;
}

[[nodiscard]] auto ForwardPass::getId() const -> PassId {
  return PassId::Forward;
}

auto ForwardPass::execute(Frame* frame, VkCommandBuffer cmdBuffer) -> void {
  ZoneScoped;
  const auto colorAttachmentInfo = aliasRegistry.getAttachmentInfo(
      "SwapchainImage",
      frame->getIndex(),
      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      VK_ATTACHMENT_LOAD_OP_CLEAR,
      VK_ATTACHMENT_STORE_OP_STORE,
      VkClearValue{.color = {.float32 = {0.392f, 0.584f, 0.929f, 1.0f}}});

  const auto depthAttachmentInfo = aliasRegistry.getAttachmentInfo(
      "DepthBuffer",
      frame->getIndex(),
      VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
      VK_ATTACHMENT_LOAD_OP_CLEAR,
      VK_ATTACHMENT_STORE_OP_STORE,
      VkClearValue{.depthStencil = VkClearDepthStencilValue{.depth = 1.f, .stencil = 0}});

  const auto renderingInfo = VkRenderingInfo{
      .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
      .renderArea = (VkRect2D{.offset = {.x = 0, .y = 0},
                              .extent = {.width = surfaceState.renderSize().width,
                                         .height = surfaceState.renderSize().height}}),
      .layerCount = 1,
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

  // Render stuff here.

  vkCmdEndRendering(cmdBuffer);
}

}
