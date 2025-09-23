#pragma once

#include "bk/NonCopyMove.hpp"

#include <readerwriterqueue.h>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace arb {

struct ImageTransitionInfo {
  VkImage image;
  VkImageLayout oldLayout;
  VkImageLayout newLayout;
  VkImageSubresourceRange subresourceRange;
};

class ImageTransitionQueue : NonCopyableMovable {
public:
  ImageTransitionQueue() = default;
  ~ImageTransitionQueue() = default;

  auto enqueue(std::vector<ImageTransitionInfo> imageTransition) -> void;
  auto dequeue() -> std::vector<ImageTransitionInfo>;

private:
  moodycamel::ReaderWriterQueue<std::vector<ImageTransitionInfo>> queue{10};
};

}
