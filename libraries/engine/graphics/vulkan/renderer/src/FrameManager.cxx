#include "FrameManager.hpp"

#include "bk/Logger.hpp"

#include "core/Device.hpp"
#include "core/Swapchain.hpp"

#include "engine/common/EngineOptions.hpp"

#include "Frame.hpp"

namespace arb {
FrameManager::FrameManager(const EngineOptions& options, Device& newDevice, Swapchain& newSwapchain)
    : device{newDevice}, swapchain{newSwapchain} {
  Log::trace("Constructing FrameManager");
  frames.reserve(FramesInFlight);

  submittedFrames.reserve(FramesInFlight);
  swapchainImageIndices.reserve(FramesInFlight);
  inFlightFences.reserve(FramesInFlight);
  imageAvailableSemaphores.reserve(FramesInFlight);
  computeFinishedSemaphores.reserve(FramesInFlight);
  lastImageUses.reserve(FramesInFlight);
  lastBufferUses.reserve(FramesInFlight);
  imageTransitionInfo.reserve(FramesInFlight);

  for (uint8_t i = 0; i < FramesInFlight; ++i) {
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
    imageTransitionInfo.emplace_back();

    frames.push_back(std::make_unique<Frame>(this, i));
  }
}

auto FrameManager::acquireFrame() -> std::variant<Frame*, ImageAcquireResult> {
  currentFrame = (currentFrame + 1) % frames.size();
  auto* frame = frames[currentFrame].get();

  if (frame->isSubmitted()) {
    const uint64_t timeout = 1'000'000; // 1ms
    VkResult result{};
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
  device.waitIdle();
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

auto FrameManager::setFrameLastImageUse(uint8_t index, const std::string& alias, LastImageUse use)
    -> void {
  lastImageUses[index][alias] = use;
}

auto FrameManager::getFrameLastImageUse(uint8_t index, const std::string& alias) -> LastImageUse {
  return lastImageUses[index][alias];
}

auto FrameManager::setFrameLastBufferUse(uint8_t index, const std::string& alias, LastBufferUse use)
    -> void {
  lastBufferUses[index][alias] = use;
}

auto FrameManager::getFrameLastBufferUse(uint8_t index, const std::string& alias) -> LastBufferUse {
  return lastBufferUses[index][alias];
}

auto FrameManager::setImageTransitionInfo(uint8_t index,
                                          const std::vector<ImageTransitionInfo>& transitionInfo)
    -> void {
  imageTransitionInfo[index] = transitionInfo;
}

auto FrameManager::getImageTransitionInfo(uint8_t index) -> std::vector<ImageTransitionInfo> {
  return imageTransitionInfo[index];
}

}
