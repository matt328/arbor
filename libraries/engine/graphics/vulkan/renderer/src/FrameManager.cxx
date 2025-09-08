#include "FrameManager.hpp"

#include "core/Device.hpp"
#include "core/Swapchain.hpp"

#include "graphics/common/GraphicsOptions.hpp"

#include "Frame.hpp"

namespace arb {
FrameManager::FrameManager(const GraphicsOptions& graphicsOptions,
                           Device& newDevice,
                           Swapchain& newSwapchain)
    : device{newDevice}, swapchain{newSwapchain} {
  Log->trace("Constructing FrameManager");
  frames.reserve(graphicsOptions.framesInFlight);
  for (uint8_t i = 0; i < graphicsOptions.framesInFlight; ++i) {
    const auto fenceName = std::format("Fence-InFlight-Frame-{}", i);
    auto fence = Fence{newDevice, fenceName};

    const auto imageAvailableName = std::format("Semaphore-ImageAvailable-Frame-{}", i);
    auto imageAvailable = Semaphore{newDevice, imageAvailableName};

    const auto computeFinishedName = std::format("Semaphore-ComputeFinished-Frame-{}", i);
    auto computeFinished = Semaphore{newDevice, computeFinishedName};

    frames.push_back(std::make_unique<Frame>(i,
                                             std::move(fence),
                                             std::move(imageAvailable),
                                             std::move(computeFinished)));
  }
}

auto FrameManager::acquireFrame() -> std::variant<Frame*, ImageAcquireResult> {
  currentFrame = (currentFrame + 1) % frames.size();
  auto* frame = frames[currentFrame].get();

  if (frame->isSubmitted()) {
    const uint64_t timeout = 1'000'000; // 1ms
    VkResult result;
    do {
      VkFence fence = frame->getInflightFence();
      result = vkWaitForFences(device, 1, &fence, VK_TRUE, timeout);
    } while (result == VK_TIMEOUT);

    if (result == VK_SUCCESS) {
      VkFence fence = frame->getInflightFence();
      vkResetFences(device, 1, &fence);
      frame->setSubmitted(false);
    }
  }

  std::variant<uint32_t, ImageAcquireResult> result{};

  result = swapchain.acquireNextImage(frame->getImageAvailableSemaphore());

  if (std::holds_alternative<uint32_t>(result)) {
    frame->setSwapchainImageIndex(std::get<uint32_t>(result));
    return frame;
  }

  const auto iar = std::get<ImageAcquireResult>(result);

  if (iar == ImageAcquireResult::NeedsResize) {
    swapchain.recreate();
  }

  return iar;
}

FrameManager::~FrameManager() {
  Log->trace("Destroying FrameManager");
}

}
