#pragma once

#include "bk/NonCopyMove.hpp"
#include "ImageViewManager.hpp"
#include "common/Handles.hpp"
#include "resources/images/ImageHandle.hpp"

namespace arb {

class LogicalImageViewAllocator : public NonCopyableMovable {
public:
  LogicalImageViewAllocator(ImageViewManager& newImageViewManager, uint32_t frameCount);
  ~LogicalImageViewAllocator();

  auto createPerFrameImageView(const VkImageViewCreateInfo& ivci,
                               ImageHandle imageHandle,
                               const std::optional<std::string>& name) -> LogicalImageViewHandle;
  auto destroyImageView(LogicalImageViewHandle handle) -> void;
  [[nodiscard]] auto resolve(LogicalImageViewHandle handle, uint32_t frameIndex) const
      -> ImageViewHandle;

private:
  ImageViewManager& imageViewManager;
  uint32_t framesInFlight;

  HandleGenerator<ImageViewTag> handleGenerator;
  std::unordered_map<LogicalImageViewHandle, std::vector<ImageViewHandle>> handleTable;
};

}
