#include "FrameManager.hpp"

#include "bk/Logger.hpp"

#include "core/Device.hpp"
#include "core/Swapchain.hpp"

#include "graphics/common/GraphicsOptions.hpp"

#include "Frame.hpp"

namespace arb {
FrameManager::FrameManager(const GraphicsOptions& graphicsOptions,
                           Device& newDevice,
                           Swapchain& newSwapchain)
    : device{newDevice}, swapchain{newSwapchain} {
  Log::trace("Constructing FrameManager");
  frames.reserve(graphicsOptions.framesInFlight);

  submittedFrames.reserve(graphicsOptions.framesInFlight);
  swapchainImageIndices.reserve(graphicsOptions.framesInFlight);
  inFlightFences.reserve(graphicsOptions.framesInFlight);
  imageAvailableSemaphores.reserve(graphicsOptions.framesInFlight);
  computeFinishedSemaphores.reserve(graphicsOptions.framesInFlight);
  lastImageUses.reserve(graphicsOptions.framesInFlight);
  lastBufferUses.reserve(graphicsOptions.framesInFlight);

  for (uint8_t i = 0; i < graphicsOptions.framesInFlight; ++i) {
    const auto fenceName = std::format("Fence-InFlight-Frame-{}", i);
    auto fence = Fence{newDevice, fenceName};

    const auto imageAvailableName = std::format("Semaphore-ImageAvailable-Frame-{}", i);
    auto imageAvailable = Semaphore{newDevice, false, imageAvailableName};

    const auto computeFinishedName = std::format("Semaphore-ComputeFinished-Frame-{}", i);
    auto computeFinished = Semaphore{newDevice, false, computeFinishedName};

    submittedFrames.push_back(false);
    swapchainImageIndices.push_back(0);
    inFlightFences.push_back(std::move(fence));
    imageAvailableSemaphores.push_back(std::move(imageAvailable));
    computeFinishedSemaphores.push_back(std::move(computeFinished));
    lastImageUses.emplace_back();
    lastBufferUses.emplace_back();

    frames.push_back(std::make_unique<Frame>(this, i));
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
  Log::trace("Destroying FrameManager");
}

auto FrameManager::setSubmittedFrame(uint8_t index, bool newSubmitted) -> void {
  submittedFrames[index] = newSubmitted;
}

auto FrameManager::isSubmittedFrame(uint8_t index) -> bool {
  return submittedFrames[index];
}

auto FrameManager::getFrameInflightFence(uint8_t index) -> Fence& {
  return inFlightFences[index];
}

auto FrameManager::getFrameImageAvailableSemaphore(uint8_t index) -> Semaphore& {
  return imageAvailableSemaphores[index];
}

auto FrameManager::getFrameComputeFinishedSemaphore(uint8_t index) -> Semaphore& {
  return computeFinishedSemaphores[index];
}

auto FrameManager::setFrameSwapchainIndex(uint8_t index, uint32_t imageIndex) -> void {
  swapchainImageIndices[index] = imageIndex;
}

auto FrameManager::getFrameSwapchainIndex(uint8_t index) -> uint32_t {
  return swapchainImageIndices[index];
}

auto FrameManager::setFrameLastImageUse(uint8_t index, ImageAlias alias, LastImageUse use) -> void {
  lastImageUses[index][alias] = use;
}

auto FrameManager::getFrameLastImageUse(uint8_t index, ImageAlias alias) -> LastImageUse {
  return lastImageUses[index][alias];
}

auto FrameManager::setFrameLastBufferUse(uint8_t index, BufferAliasVariant alias, LastBufferUse use)
    -> void {
  lastBufferUses[index][alias] = use;
}

auto FrameManager::getFrameLastBufferUse(uint8_t index, BufferAliasVariant alias) -> LastBufferUse {
  return lastBufferUses[index][alias];
}

}
