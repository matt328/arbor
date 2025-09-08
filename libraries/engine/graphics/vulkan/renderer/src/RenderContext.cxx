#include "renderer/RenderContext.hpp"

#include "bk/Logger.hpp"

#include "graphics/common/GraphicsOptions.hpp"

#include "core/Device.hpp"
#include "core/Swapchain.hpp"

#include "FrameManager.hpp"
#include "PerFrameUploader.hpp"

namespace arb {

RenderContext::RenderContext(const GraphicsOptions& graphicsOptions,
                             Device& newDevice,
                             Swapchain& newSwapchain,
                             IStateBuffer<SimState>& simStateBuffer)
    : device{newDevice}, swapchain{newSwapchain} {
  Log::trace("Creating RenderContext");
  frameManager = std::make_unique<FrameManager>(graphicsOptions, device, swapchain);
  perFrameUploader = std::make_unique<PerFrameUploader>(simStateBuffer);
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
