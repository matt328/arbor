#include "Frame.hpp"

#include "bk/Logger.hpp"

namespace arb {

Frame::Frame(uint8_t newIndex,
             Fence&& newRenderFence,
             Semaphore&& newImageAvailableSemaphore,
             Semaphore&& newComputeFinishedSemaphore)
    : index{newIndex},
      imageAvailableSemaphore{std::move(newImageAvailableSemaphore)},
      computeFinishedSemaphore{std::move(newComputeFinishedSemaphore)} {
  Log::trace("Creating Frame index={}", newIndex);
}

Frame::~Frame() {
  Log::trace("Destroying Frame index={}", index);
}

}
