#include "ImageTransitionQueue.hpp"

#include "bk/Log.hpp"

namespace arb {

auto ImageTransitionQueue::enqueue(std::vector<ImageTransitionInfo> imageTransition) -> void {
  queue.enqueue(std::move(imageTransition));
}

auto ImageTransitionQueue::dequeue() -> std::vector<ImageTransitionInfo> {
  auto batch = std::vector<ImageTransitionInfo>{};
  queue.try_dequeue(batch);
  if (!batch.empty()) {
    LOG_TRACE_L1(Log::Renderer, "ImageTransitionQueue not empty, size={}", batch.size());
  }
  return batch;
}

}
