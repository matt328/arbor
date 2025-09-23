#pragma once

#include "ImageTransitionQueue.hpp"
#include "common/Semaphore.hpp"
#include "common/Fence.hpp"
#include "FrameManager.hpp"

namespace arb {

class Frame {
public:
  Frame(FrameManager* newFrameManager, uint8_t newIndex);
  ~Frame();

  auto setSubmitted(bool newSubmitted) {
    frameManager->setSubmittedFrame(index, newSubmitted);
  }

  [[nodiscard]] auto isSubmitted() const {
    return frameManager->isSubmittedFrame(index);
  }

  [[nodiscard]] auto getInflightFence() -> Fence& {
    return frameManager->getFrameInflightFence(index);
  }

  [[nodiscard]] auto getImageAvailableSemaphore() -> Semaphore& {
    return frameManager->getFrameImageAvailableSemaphore(index);
  }

  [[nodiscard]] auto getIndex() const {
    return index;
  }

  auto setSwapchainImageIndex(uint32_t imageIndex) {
    frameManager->setFrameSwapchainIndex(index, imageIndex);
  }

  auto getSwapchainImageIndex() {
    return frameManager->getFrameSwapchainIndex(index);
  }

  auto setLastImageUse(std::string alias, LastImageUse use) {
    frameManager->setFrameLastImageUse(index, alias, use);
  }

  auto getLastImageUse(std::string alias) {
    return frameManager->getFrameLastImageUse(index, alias);
  }

  auto setLastBufferUse(std::string alias, LastBufferUse use) {
    frameManager->setFrameLastBufferUse(index, alias, use);
  }

  auto getLastBufferUse(std::string alias) {
    return frameManager->getFrameLastBufferUse(index, alias);
  }

  auto setImageTransitionInfo(const std::vector<ImageTransitionInfo>& transitionInfo) {
    frameManager->setImageTransitionInfo(index, transitionInfo);
  }

  auto getImageTransitionInfo() {
    return frameManager->getImageTransitionInfo(index);
  }

private:
  uint8_t index;
  FrameManager* frameManager;
};

}
