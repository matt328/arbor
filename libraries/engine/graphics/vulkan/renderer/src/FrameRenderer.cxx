#include "FrameRenderer.hpp"

#include <array>
#include <Tracy.hpp>

#include "Frame.hpp"
#include "bk/Logger.hpp"

#include "common/ErrorUtils.hpp"
#include "core/Device.hpp"
#include "core/Swapchain.hpp"
#include "FrameManager.hpp"
#include "PerFrameUploader.hpp"
#include "framegraph/AliasRegistry.hpp"
#include "framegraph/FrameGraph.hpp"
#include "framegraph/render-pass/IRenderPass.hpp"
#include "vulkan/vulkan_core.h"

namespace arb {

FrameRenderer::FrameRenderer(Device& newDevice,
                             Swapchain& newSwapchain,
                             FrameManager& newFrameManager,
                             PerFrameUploader& newPerFrameUploader,
                             AliasRegistry& newAliasRegistry,
                             FrameGraph& newFrameGraph)
    : device{newDevice},
      swapchain{newSwapchain},
      frameManager{newFrameManager},
      perFrameUploader{newPerFrameUploader},
      aliasRegistry{newAliasRegistry},
      frameGraph{newFrameGraph} {
  Log::trace("Creating FrameRenderer");
}

FrameRenderer::~FrameRenderer() {
  Log::trace("Destroying FrameRenderer");
}

void FrameRenderer::renderNextFrame() {
  ZoneScoped;
  if (resizePending) {
    recreateSwapchain();
    resizePending = false;
    return;
  }

  auto* frame = tryAcquireFrame();

  if (frame == nullptr) {
    return;
  }

  perFrameUploader.upload();

  auto results = frameGraph.execute(frame);

  submitFrame(frame, results);

  const auto presentResult = presentFrame(frame);
  if (presentResult == VK_SUBOPTIMAL_KHR || presentResult == VK_ERROR_OUT_OF_DATE_KHR) {
    Log::warn("PresentFrame resports swapchain needs resized");
    resizePending = true;
  }
}

auto FrameRenderer::presentFrame(Frame* frame) -> VkResult {
  ZoneScoped;
  const auto swapchainImageIndex = frame->getSwapchainImageIndex();
  VkSemaphore swapchainImageSemaphore = swapchain.getImageSemaphore(swapchainImageIndex);
  VkSwapchainKHR chain = swapchain;
  const auto presentInfo = VkPresentInfoKHR{.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                                            .waitSemaphoreCount = 1,
                                            .pWaitSemaphores = &swapchainImageSemaphore,
                                            .swapchainCount = 1,
                                            .pSwapchains = &chain,
                                            .pImageIndices = &swapchainImageIndex};

  return vkQueuePresentKHR(device.presentQueue(), &presentInfo);
}

void FrameRenderer::recreateSwapchain() {
}

auto FrameRenderer::tryAcquireFrame() -> Frame* {
  ZoneScoped;
  const auto result = frameManager.acquireFrame();

  if (std::holds_alternative<Frame*>(result)) {
    return std::get<Frame*>(result);
  }

  switch (std::get<ImageAcquireResult>(result)) {
    case ImageAcquireResult::Error:
      Log::warn("Failed to acquire swapchain image, skipping frame");
      return nullptr;
    case ImageAcquireResult::NeedsResize:
      resizePending = true;
      return nullptr;
    default:
      Log::warn("Unexpected result acquiring swapchain image");
      return nullptr;
  }
}

void FrameRenderer::submitFrame(Frame* frame, const FrameGraphResult& frameResult) {
  ZoneScoped;
  frame->setSubmitted(true);

  constexpr auto waitStages =
      std::array<VkPipelineStageFlags, 1>{VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

  const auto swapchainImageIndex = frame->getSwapchainImageIndex();
  VkSemaphore swapchainImageSemaphore = swapchain.getImageSemaphore(swapchainImageIndex);

  VkSemaphore imageAvailableSemaphore = frame->getImageAvailableSemaphore();

  const auto submitInfo = VkSubmitInfo{
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &imageAvailableSemaphore,
      .pWaitDstStageMask = waitStages.data(),
      .commandBufferCount = static_cast<uint32_t>(frameResult.commandBuffers.size()),
      .pCommandBuffers = frameResult.commandBuffers.data(),
      .signalSemaphoreCount = 1,
      .pSignalSemaphores = &swapchainImageSemaphore,
  };

  auto* graphicsQueue = device.graphicsQueue();
  checkVk(vkQueueSubmit(graphicsQueue, 1, &submitInfo, frame->getInflightFence()), "vkQueueSubmit");
  // frameState->advanceFrame();
}

}
