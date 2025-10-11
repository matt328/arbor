#include "LogicalImageViewAllocator.hpp"

#include <cassert>

#include "bk/Log.hpp"
#include "core/ImageHandle.hpp"

namespace arb {

LogicalImageViewAllocator::LogicalImageViewAllocator(ImageViewManager& newImageViewManager,
                                                     uint32_t frameCount)
    : imageViewManager{newImageViewManager}, framesInFlight{frameCount} {
  LOG_TRACE_L1(Log::Resources, "Creating LogicalImageViewAllocator");
}

LogicalImageViewAllocator::~LogicalImageViewAllocator() {
  LOG_TRACE_L1(Log::Resources, "Destroying LogicalImageViewAllocator");
}

auto LogicalImageViewAllocator::createPerFrameImageView(const VkImageViewCreateInfo& ivci,
                                                        ImageHandle imageHandle,
                                                        const std::optional<std::string>& name)
    -> LogicalImageViewHandle {

  const auto logicalHandle = handleGenerator.requestLogicalHandle();
  auto handleList = std::vector<ImageViewHandle>{framesInFlight};
  for (auto i = 0; i < framesInFlight; ++i) {
    const auto indexedName = std::format("{}-Frame-{}", name.value_or("Unnamed ImageView"), i);
    const auto handle = imageViewManager.createImageView(ivci, imageHandle, indexedName);
    handleList.push_back(handle);
  }
  handleTable.emplace(logicalHandle, handleList);
  return logicalHandle;
}

auto LogicalImageViewAllocator::destroyImageView(LogicalImageViewHandle handle) -> void {
  assert(handleTable.contains(handle));
  const auto handleList = handleTable.at(handle);

  for (const auto pHandle : handleList) {
    imageViewManager.destroyImageView(pHandle);
  }

  handleTable.erase(handle);
}

auto LogicalImageViewAllocator::resolve(LogicalImageViewHandle handle, uint32_t frameIndex) const
    -> ImageViewHandle {
  assert(handleTable.contains(handle));
  return handleTable.at(handle).at(frameIndex);
}

}
