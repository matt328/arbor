#pragma once

#include "bk/NonCopyMove.hpp"
#include "engine/common/HandleGenerator.hpp"
#include "core/ImageHandle.hpp"
#include "ImageManager.hpp"

namespace arb {

class LogicalImageAllocator : public NonCopyableMovable {
public:
  explicit LogicalImageAllocator(ImageManager& newImageManager, uint32_t frameCount);
  ~LogicalImageAllocator();

  auto createPerFrameImage(const VkImageCreateInfo& ici,
                           const VmaAllocationCreateInfo& aci,
                           const std::optional<std::string>& name) -> LogicalImageHandle;
  auto destroyImage(LogicalImageHandle handle) -> void;
  [[nodiscard]] auto resolve(LogicalImageHandle handle, uint32_t frameIndex) const -> ImageHandle;

private:
  ImageManager& imageManager;
  uint32_t framesInFlight;

  HandleGenerator<ImageTag> handleGenerator;
  std::unordered_map<LogicalImageHandle, std::vector<ImageHandle>> handleTable;
};

}
