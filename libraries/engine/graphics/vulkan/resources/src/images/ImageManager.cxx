#include "ImageManager.hpp"

#include "bk/Log.hpp"
#include "common/ResizePolicy.hpp"

#include <cassert>

namespace arb {

ImageManager::ImageManager(Device& newDevice, AllocatorService& newAllocatorService)
    : device{newDevice}, allocatorService{newAllocatorService} {
  LOG_TRACE_L1(Log::Resources, "Creating ImageManager");
}

ImageManager::~ImageManager() {
  LOG_TRACE_L1(Log::Resources, "Destroying ImageManager");
}

void ImageManager::resize(const RenderSurfaceState& newState) {
  for (auto& [handle, image] : imageMap) {
    auto newExtent = newState.swapchainExtent;
    if (image->getResizePolicy() == ResizePolicy::RenderScale) {
      newExtent = newState.renderSize();
    }
    image->resize(VkExtent2D{.width = newExtent.width, .height = newExtent.height});
  }
}

auto ImageManager::createImage(const VkImageCreateInfo& ici,
                               const VmaAllocationCreateInfo& aci,
                               ResizePolicy resizePolicy,
                               const std::optional<std::string>& name) -> ImageHandle {
  const auto handle = handleGenerator.requestHandle();
  imageMap.emplace(handle,
                   std::make_unique<Image>(&device,
                                           &allocatorService,
                                           ici,
                                           aci,
                                           resizePolicy,
                                           name.value_or("Unnamed Image")));
  return handle;
}

auto ImageManager::destroyImage(ImageHandle handle) -> void {
  assert(imageMap.contains(handle));
  imageMap.erase(handle);
}

auto ImageManager::getImage(ImageHandle handle) -> Image& {
  assert(imageMap.contains(handle));
  return *imageMap.at(handle);
}

auto ImageManager::incrementImageUse(ImageHandle handle) -> void {
  ++usesMap[handle];
}

auto ImageManager::decrementImageUse(ImageHandle handle) -> void {
  --usesMap[handle];
}

}
