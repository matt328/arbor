#include "ForwardPass.hpp"

#include "Frame.hpp"
#include "bk/Logger.hpp"
#include <vulkan/vulkan_core.h>

namespace arb {

ForwardPass::ForwardPass(const ForwardPassContext& ctx, const ForwardPassConfig& config)
    : aliasRegistry{ctx.aliasRegistry},
      resourceSystem{ctx.resourceSystem},
      dispatcherFactory{ctx.dispatcherFactory},
      pipelineManager{ctx.pipelineManager},
      pipelineHandle{config.pipelineHandle},
      dispatcherHandles{config.dispatcherHandles},
      colorImage{config.colorImage},
      depthImage{config.depthImage} {
  Log::trace("Creating ForwardPass");
}

ForwardPass::~ForwardPass() {
  Log::trace("Destroying ForwardPass");
}

[[nodiscard]] auto ForwardPass::getId() const -> PassId {
}

auto ForwardPass::execute(Frame* frame, VkCommandBuffer cmdBuffer) -> void {
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

  for (const auto& handle : dispatcherHandles) {
    auto& dispatcher = dispatcherFactory.getDispatcher(handle);
    dispatcher.bind(frame, cmdBuffer, pipelineUnit.layout);
    dispatcher.dispatch(frame, cmdBuffer);
  }

  vkCmdEndRendering(cmdBuffer);
}

auto ForwardPass::registerDispatchContext(DispatcherHandle handle) -> void {
  dispatcherHandles.push_back(handle);
}

}
