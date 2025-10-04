#pragma once

#include "core/AllocatorService.hpp"
#include "engine/common/HandleGenerator.hpp"
#include "core/Image.hpp"
#include "core/ImageHandle.hpp"
#include <vulkan/vulkan_core.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"
#include <vma/vk_mem_alloc.h>
#pragma clang diagnostic pop

namespace arb {

class ImageManager {
public:
  explicit ImageManager(Device& newDevice, AllocatorService& newAllocatorService);
  ~ImageManager();

  auto createImage(const VkImageCreateInfo& ici,
                   const VmaAllocationCreateInfo& aci,
                   const std::optional<std::string>& name) -> ImageHandle;
  auto destroyImage(ImageHandle handle) -> void;
  auto getImage(ImageHandle handle) -> Image&;
  auto incrementImageUse(ImageHandle handle) -> void;
  auto decrementImageUse(ImageHandle handle) -> void;

private:
  Device& device;
  AllocatorService& allocatorService;

  HandleGenerator<ImageTag> handleGenerator;
  std::unordered_map<ImageHandle, std::unique_ptr<Image>> imageMap;
  std::unordered_map<ImageHandle, uint32_t> usesMap;
};

}
