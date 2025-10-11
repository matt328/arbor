#include "FrameRenderer.hpp"

#include <array>
#include <expected>
#include <Tracy.hpp>
#include <utility>

#include "Frame.hpp"

#include "bk/Log.hpp"

#include "common/ErrorUtils.hpp"
#include "core/Device.hpp"
#include "core/Swapchain.hpp"
#include "FrameManager.hpp"
#include "PerFrameUploader.hpp"
#include "engine/common/RenderSurfaceState.hpp"
#include "framegraph/AliasRegistry.hpp"
#include "framegraph/FrameGraph.hpp"
#include "framegraph/render-pass/IRenderPass.hpp"

#include <vulkan/vulkan_core.h>

namespace arb {

FrameRenderer::FrameRenderer(const FrameRendererDeps& deps, const FrameRendererCreateInfo& info)
    : device{deps.device},
      swapchain{deps.swapchain},
      frameManager{deps.frameManager},
      perFrameUploader{deps.perFrameUploader},
      aliasRegistry{deps.aliasRegistry},
      frameGraph{deps.frameGraph} {
  LOG_TRACE_L1(Log::Renderer, "Creating FrameRenderer");
}

FrameRenderer::~FrameRenderer() {
  LOG_TRACE_L1(Log::Renderer, "Destroying FrameRenderer");
}

void FrameRenderer::setOnSwapchainResized(std::function<void(RenderSurfaceState)> cb) {
  onResize = std::move(cb);
}

void FrameRenderer::renderNextFrame() {
  ZoneScoped;

  if (resizeInProgress()) {
    device.waitIdle();
    swapchain.recreate();
    auto newSurfaceState =
        RenderSurfaceState{.swapchainExtent = Size{.width = swapchain.getExtent().width,
                                                   .height = swapchain.getExtent().height}};
    frameGraph.resize(newSurfaceState);
    onResize(newSurfaceState);
    commitResize();
  }

  auto frameResult = tryAcquireFrame();
  if (!frameResult.has_value()) {
    if (frameResult.error() == FrameStatus::NeedsResize) {
      beginResize();
    }
    return;
  }

  auto* frame = *frameResult;

  perFrameUploader.upload(frame);

  auto results = frameGraph.execute(frame);

  submitFrame(frame, results);

  const auto presentResult = presentFrame(frame);

  switch (presentResult) {
    case FrameStatus::SwapchainOutOfDate:
    case FrameStatus::SwapchainSuboptimal:
      LOG_WARNING(Log::Renderer, "PresentFrame reports swapchain needs resized");
      beginResize();
      break;
    case FrameStatus::PresentFailed:
      LOG_WARNING(Log::Renderer, "Failed to present frame");
      break;
    default:
      break;
  }
}

auto FrameRenderer::tryAcquireFrame() -> std::expected<Frame*, FrameStatus> {
  ZoneScoped;
  const auto result = frameManager.acquireFrame();

  if (std::holds_alternative<Frame*>(result)) {
    return std::get<Frame*>(result);
  }

  switch (std::get<ImageAcquireResult>(result)) {
    case ImageAcquireResult::Error:
      LOG_WARNING(Log::Renderer, "Failed to acquire swapchain image, skipping frame");
      return std::unexpected(FrameStatus::AcquireFailed);
    case ImageAcquireResult::NeedsResize:
      LOG_WARNING(Log::Renderer, "Image Acquisition reports swapchain needs resized");
      return std::unexpected(FrameStatus::NeedsResize);
    default:
      LOG_WARNING(Log::Renderer, "Unexpected result acquiring swapchain image");
      return std::unexpected(FrameStatus::AcquireFailed);
  }
}

auto FrameRenderer::presentFrame(Frame* frame) -> FrameStatus {
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

  const auto result = vkQueuePresentKHR(device.presentQueue(), &presentInfo);

  switch (result) {
    case VK_SUCCESS:
      return FrameStatus::Success;
    case VK_SUBOPTIMAL_KHR:
      return FrameStatus::SwapchainSuboptimal;
    case VK_ERROR_OUT_OF_DATE_KHR:
      return FrameStatus::SwapchainOutOfDate;
    default:
      LOG_WARNING(Log::Renderer, "vkPresentQueueKHR failed with error code {}", int(result));
      return FrameStatus::PresentFailed;
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

void FrameRenderer::beginResize() {
  resizePending = true;
  device.waitIdle();
}

void FrameRenderer::commitResize() {
  resizePending = false;
}

auto FrameRenderer::resizeInProgress() const noexcept -> bool {
  return resizePending;
}

}
