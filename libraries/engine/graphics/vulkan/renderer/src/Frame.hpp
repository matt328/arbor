#pragma once

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

  auto setLastImageUse(ImageAlias alias, LastImageUse use) {
    frameManager->setFrameLastImageUse(index, alias, use);
  }

  auto getLastImageUse(ImageAlias alias) {
    return frameManager->getFrameLastImageUse(index, alias);
  }

  auto setLastBufferUse(BufferAliasVariant alias, LastBufferUse use) {
    frameManager->setFrameLastBufferUse(index, alias, use);
  }

  auto getLastBufferUse(BufferAliasVariant alias) {
    return frameManager->getFrameLastBufferUse(index, alias);
  }

private:
  uint8_t index;
  FrameManager* frameManager;
};

}
