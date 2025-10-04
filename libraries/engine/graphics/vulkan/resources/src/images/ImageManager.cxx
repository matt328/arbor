#include "ImageManager.hpp"

#include "bk/Logger.hpp"

#include <cassert>

namespace arb {

ImageManager::ImageManager(Device& newDevice, AllocatorService& newAllocatorService)
    : device{newDevice}, allocatorService{newAllocatorService} {
  Log::trace("Creating ImageManager");
}

ImageManager::~ImageManager() {
  Log::trace("Destroying ImageManager");
}

auto ImageManager::createImage(const VkImageCreateInfo& ici,
                               const VmaAllocationCreateInfo& aci,
                               const std::optional<std::string>& name) -> ImageHandle {
  const auto handle = handleGenerator.requestHandle();
  imageMap.emplace(handle,
                   std::make_unique<Image>(&device,
                                           &allocatorService,
                                           ici,
                                           aci,
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
