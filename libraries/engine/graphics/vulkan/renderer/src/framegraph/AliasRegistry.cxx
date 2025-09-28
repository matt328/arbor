#include "AliasRegistry.hpp"

#include "bk/Logger.hpp"
#include "resources/images/ImageSpec.hpp"
#include "vulkan/vulkan_core.h"

namespace arb {

AliasRegistry::AliasRegistry(ResourceSystem& newResourceSystem)
    : resourceSystem{newResourceSystem} {
  Log::trace("Creating AliasRegistry");
}

void AliasRegistry::registerImageAlias(const std::string& alias, ImageSpec spec) {
  Log::trace("Registering ImageAlias {}", alias);
  if (aliasImageSpecMap.contains(alias)) {
    if (!(aliasImageSpecMap.at(alias) == spec)) {
      throw std::runtime_error("Alias " + alias + " registered with a different spec");
    }
    Log::trace("Alias already existd: {}", alias);
    return;
  }
  Log::trace("Registering new alias {}", alias);
  aliasImageSpecMap.emplace(alias, spec);
}

void AliasRegistry::buildResources(uint32_t frameCount) {
  for (const auto& [alias, spec] : aliasImageSpecMap) {

    uint32_t instances = 1;
    switch (spec.imageLifetime) {
      case ImageLifetime::Persistent:
        instances = 1;
        break;
      case ImageLifetime::Transient:
        instances = frameCount;
        break;
      case ImageLifetime::Swapchain:
        instances = 3;
        break;
    }

    aliasImageViewMap[alias].resize(instances);
    for (uint32_t i = 0; i < instances; ++i) {
      const auto imgHandle = resourceSystem.createImage(spec);
      const auto imageViewSpec = createImageViewSpec(imgHandle, spec);
      const auto imageViewHandle = resourceSystem.createImageView(imageViewSpec);
      aliasImageViewMap[alias][i] = imageViewHandle;
    }
  }
}

auto AliasRegistry::getImage(std::string_view alias, uint32_t frameIndex) const -> Image& {
}

auto AliasRegistry::getImageHandle(std::string_view alias, uint32_t frameIndex) const
    -> ImageHandle {
}

auto AliasRegistry::getBufferHandle(std::string_view alias, uint32_t frameIndex) const
    -> BufferHandle {
}

auto AliasRegistry::getBuffer(std::string_view alias, uint32_t frameIndex) const -> Buffer& {
}

auto AliasRegistry::getAttachmentInfo(const std::string& alias,
                                      uint32_t frameIndex,
                                      VkImageLayout layout,
                                      VkAttachmentLoadOp loadOp,
                                      VkAttachmentStoreOp storeOp,
                                      std::optional<VkClearValue> clearValue) const
    -> VkRenderingAttachmentInfo {
  const auto& handles = aliasImageViewMap.at(alias);
  const auto& vkView = resourceSystem.getImageView(handles.at(frameIndex));
  auto info = VkRenderingAttachmentInfo{.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
                                        .imageView = vkView,
                                        .imageLayout = layout,
                                        .loadOp = loadOp,
                                        .storeOp = storeOp};
  if (clearValue.has_value()) {
    info.clearValue = clearValue.value();
  }
  return info;
}

auto AliasRegistry::createImageViewSpec(ImageHandle imageHandle, const ImageSpec& imageSpec)
    -> ImageViewSpec {
  ImageViewSpec viewSpec{};
  viewSpec.image = imageHandle;
  viewSpec.format = imageSpec.format;

  if ((imageSpec.usageFlags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0u) {
    viewSpec.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    if (((imageSpec.format & VK_FORMAT_D32_SFLOAT_S8_UINT) != 0u) ||
        (imageSpec.format & VK_FORMAT_D24_UNORM_S8_UINT) != 0u) {
      viewSpec.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }
    viewSpec.viewType = VK_IMAGE_VIEW_TYPE_2D;
  } else {
    viewSpec.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewSpec.viewType =
        (imageSpec.layers == 1) ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_2D_ARRAY;
  }

  viewSpec.baseMipLevel = 0;
  viewSpec.levelCount = imageSpec.mipLevels;
  viewSpec.baseArrayLayer = 0;
  viewSpec.layerCount = imageSpec.layers;
  viewSpec.debugName = imageSpec.debugName;

  return viewSpec;
}

}
