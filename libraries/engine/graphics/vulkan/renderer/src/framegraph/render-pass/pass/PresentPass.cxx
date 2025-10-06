#include "PresentPass.hpp"
#include "common/ImageRequirement.hpp"
#include "vulkan/vulkan_core.h"
#include <optional>

namespace arb {

PresentPass::PresentPass(const PresentPassConfig& config)
    : surfaceState{config.initialSurfaceState} {
}

[[nodiscard]] auto PresentPass::getId() const -> PassId {
  return PassId::Present;
}

void PresentPass::resize(const RenderSurfaceState& newState) {
  // NOOP
}

auto PresentPass::execute(Frame* frame, VkCommandBuffer cmdBuffer) -> void {
  // NOOP
}

auto PresentPass::getDescription() const -> PassDescription {
  auto desc = PassDescription{
      .name = "PresentPass",
      .images = {
          ImageRequirement{.alias = "SwapchainImage",
                           .createDesc = std::make_optional<ImageCreateDescription>(
                               {.format = VK_FORMAT_R8G8B8A8_UNORM,
                                .extent = {.width = surfaceState.swapchainExtent.width,
                                           .height = surfaceState.swapchainExtent.height},
                                .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                .imageLifetime = ImageLifetime::Swapchain}),
                           .useDesc = {.accessFlags = VK_ACCESS_2_NONE,
                                       .stageFlags = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
                                       .imageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                       .aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT}},
      }};
  return desc;
}
}
