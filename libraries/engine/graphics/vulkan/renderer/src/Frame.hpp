#pragma once

#include "common/Semaphore.hpp"
#include "common/Fence.hpp"

namespace arb {

class Frame {
public:
  Frame(uint8_t newIndex,
        Fence&& newRenderFence,
        Semaphore&& newImageAvailableSemaphore,
        Semaphore&& newComputeFinishedSemaphore);
  ~Frame();

  auto setSubmitted(bool newSubmitted) {
    submitted = newSubmitted;
  }

  [[nodiscard]] auto isSubmitted() const {
    return submitted;
  }

  [[nodiscard]] auto getInflightFence() -> Fence& {
    return inFlightFence;
  }

  [[nodiscard]] auto getImageAvailableSemaphore() -> Semaphore& {
    return imageAvailableSemaphore;
  }

  auto setSwapchainImageIndex(uint32_t imageIndex) {
    swapchainImageIndex = imageIndex;
  }

private:
  uint8_t index;
  bool submitted{};

  uint32_t swapchainImageIndex;

  Fence inFlightFence;
  Semaphore imageAvailableSemaphore;
  Semaphore computeFinishedSemaphore;
};

}
