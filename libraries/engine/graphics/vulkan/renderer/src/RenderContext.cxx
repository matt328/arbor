#include "renderer/RenderContext.hpp"

#include "bk/Logger.hpp"

#include "core/Device.hpp"
#include "core/Swapchain.hpp"
#include "core/command-buffers/CommandBufferManager.hpp"
#include "resources/ResourceFacade.hpp"

#include "graphics/common/GraphicsOptions.hpp"

#include "framegraph/FrameGraph.hpp"

#include "FrameManager.hpp"
#include "PerFrameUploader.hpp"
#include "framegraph/AliasRegistry.hpp"

namespace arb {

RenderContext::RenderContext(const GraphicsOptions& graphicsOptions,
                             Device& newDevice,
                             Swapchain& newSwapchain,
                             IStateBuffer<SimState>& simStateBuffer,
                             GeometryHandleMapper& newGeometryHandleMapper,
                             CommandBufferManager& newCommandBufferManager,
                             ResourceFacade& newResourceFacade)
    : device{newDevice}, swapchain{newSwapchain} {
  Log::trace("Creating RenderContext");
  frameManager = std::make_unique<FrameManager>(graphicsOptions, device, swapchain);
  perFrameUploader = std::make_unique<PerFrameUploader>(simStateBuffer, newGeometryHandleMapper);
  aliasRegistry = std::make_unique<AliasRegistry>();
  frameGraph =
      std::make_unique<FrameGraph>(newCommandBufferManager, newResourceFacade, *aliasRegistry);
}

RenderContext::~RenderContext() {
  Log::trace("Destroying RenderContext");
}

auto RenderContext::renderNextFrame() -> void {
  if (resizePending) {
    recreateSwapchain();
    resizePending = false;
    return;
  }

  auto* frame = tryAcquireFrame();

  if (frame == nullptr) {
    return;
  }

  perFrameUploader->upload();

  auto results = frameGraph->execute(frame);

  // submitFrame(frame, results);

  // const auto presentResult = presentFrame(frame);
  // if (presentResult == VK_SUBOPTIMAL_KHR || presentResult == VK_ERROR_OUT_OF_DATE_KHR) {
  //   Log::warn("PresentFrame resports swapchain needs resized");
  //   resizePending = true;
  // }
  // FrameMark;
}

auto RenderContext::recreateSwapchain() -> void {
}

/// Blocks until a frame from the frames in flight pool is finished rendering the previous frame and
/// ready to render another. If the swapchain needs resized, it returns nullptr and sets
/// resizePending. If any other error occurs, it returns nullptr and logs a warning and we should
/// skip this frame.
auto RenderContext::tryAcquireFrame() -> Frame* {
  const auto result = frameManager->acquireFrame();

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

}
