#include "LogicalImageAllocator.hpp"

#include <cassert>

#include "bk/Log.hpp"
#include "common/ResizePolicy.hpp"

namespace arb {

LogicalImageAllocator::LogicalImageAllocator(ImageManager& newImageManager, uint32_t frameCount)
    : imageManager{newImageManager}, framesInFlight{frameCount} {
  LOG_TRACE_L1(Log::Resources, "Creating LogicalImageAllocator");
}

LogicalImageAllocator::~LogicalImageAllocator() {
  LOG_TRACE_L1(Log::Resources, "Destroying LogicalImageAllocator");
}

auto LogicalImageAllocator::createPerFrameImage(const VkImageCreateInfo& ici,
                                                const VmaAllocationCreateInfo& aci,
                                                const std::optional<std::string>& name)
    -> LogicalImageHandle {
  const auto logicalHandle = handleGenerator.requestLogicalHandle();
  auto handleList = std::vector<ImageHandle>{framesInFlight};
  for (auto i = 0; i < framesInFlight; ++i) {
    auto indexedName = std::format("{}-Frame-{}", name.value_or("Unnamed Image"), i);
    const auto handle = imageManager.createImage(ici, aci, ResizePolicy::Fixed, indexedName);
    handleList.push_back(handle);
  }
  handleTable.emplace(logicalHandle, handleList);
  return logicalHandle;
}

auto LogicalImageAllocator::destroyImage(LogicalImageHandle handle) -> void {
  assert(handleTable.contains(handle));
  const auto handleList = handleTable.at(handle);

  for (const auto pHandle : handleList) {
    imageManager.destroyImage(pHandle);
  }

  handleTable.erase(handle);
}

auto LogicalImageAllocator::resolve(LogicalImageHandle handle, uint32_t frameIndex) const
    -> ImageHandle {
  assert(handleTable.contains(handle));
  return handleTable.at(handle).at(frameIndex);
}

}
