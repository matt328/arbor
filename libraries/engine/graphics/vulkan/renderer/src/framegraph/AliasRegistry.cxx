#include "AliasRegistry.hpp"

#include <cassert>
#include <cpptrace/cpptrace.hpp>
#include <cpptrace/exceptions.hpp>
#include <vulkan/vulkan_core.h>

#include "bk/Logger.hpp"
#include "common/ImageCreateDescription.hpp"
#include "common/ImageLifetime.hpp"
#include "renderer/Constants.hpp"
#include "Frame.hpp"

namespace arb {

AliasRegistry::AliasRegistry(const AliasRegistryDeps& deps)
    : resourceSystem{deps.resourceSystem}, swapchain{deps.swapchain} {
  Log::trace("Creating AliasRegistry");
}

void AliasRegistry::registerImageAlias(const std::string& alias, ImageCreateDescription desc) {
  Log::trace("Registering ImageAlias {}", alias);
  if (aliasImageSpecMap.contains(alias)) {
    if (!(aliasImageSpecMap.at(alias) == desc)) {
      throw cpptrace::runtime_error("Alias " + alias + " registered with a different spec");
    }
    Log::trace("Alias already existd: {}", alias);
    return;
  }
  Log::trace("Registering new alias {}", alias);
  aliasImageSpecMap.emplace(alias, desc);
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

    aliasImageViewMap[alias].imageViewHandles.resize(instances);
    aliasImageHandleMap[alias].imageHandles.resize(instances);
    for (uint32_t i = 0; i < instances; ++i) {

      const auto imgHandle = resourceSystem.createImage(spec);
      aliasImageHandleMap[alias].lifetime = spec.imageLifetime;
      aliasImageHandleMap[alias].imageHandles[i] = imgHandle;

      const auto imageViewSpec = createImageViewSpec(imgHandle, spec);
      const auto imageViewHandle = resourceSystem.createImageView(imageViewSpec);

      aliasImageViewMap[alias].lifetime = spec.imageLifetime;
      aliasImageViewMap[alias].imageViewHandles[i] = imageViewHandle;
    }
  }
}

auto AliasRegistry::getImage(const std::string& alias, Frame* frame) const -> const Image& {
  if (alias == Constants::SwapchainAlias) {
    return getSwapchainImage(frame->getSwapchainImageIndex());
  }
  const auto imgHandle = getImageHandle(alias, frame);
  return resourceSystem.getImage(imgHandle);
}

auto AliasRegistry::getImageHandle(const std::string& alias, Frame* frame) const -> ImageHandle {
  if (alias == Constants::SwapchainAlias) {
    return getSwapchainImageHandle(frame->getSwapchainImageIndex());
  }
  if (aliasImageHandleMap.contains(alias)) {
    const auto& entry = aliasImageHandleMap.at(alias);
    switch (entry.lifetime) {
      case ImageLifetime::Persistent:
        return entry.imageHandles[0];
      case ImageLifetime::Transient:
      case ImageLifetime::Swapchain:
        assert(frame->getIndex() < entry.imageHandles.size());
        return entry.imageHandles[frame->getIndex()];
    }
    throw cpptrace::logic_error("Unhandled ImageLifetime");
  }

  throw cpptrace::logic_error(std::format("Requested alias {} not registered", alias));
}

auto AliasRegistry::getSwapchainImage(uint32_t index) const -> Image& {
  return swapchain.getImage(index);
}

auto AliasRegistry::getSwapchainImageHandle(uint32_t index) const -> ImageHandle {
  return swapchain.getImageHandle(index);
}

auto AliasRegistry::getImageViewHandle(const std::string& alias, uint32_t frameIndex) const
    -> ImageViewHandle {
  if (aliasImageViewMap.contains(alias)) {
    const auto& entry = aliasImageViewMap.at(alias);
    switch (entry.lifetime) {
      case ImageLifetime::Persistent:
        return entry.imageViewHandles[0];
      case ImageLifetime::Transient:
      case ImageLifetime::Swapchain:
        assert(frameIndex < entry.imageViewHandles.size());
        return entry.imageViewHandles[frameIndex];
    }
    throw cpptrace::logic_error("Unhandled ImageLifetime");
  }

  throw cpptrace::logic_error(std::format("Requested alias {} not registered", alias));
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
  const auto& imageViewHandle = getImageViewHandle(alias, frameIndex);
  const auto& vkView = resourceSystem.getImageView(imageViewHandle);
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

auto AliasRegistry::createImageViewSpec(ImageHandle imageHandle, const ImageCreateDescription& desc)
    -> ImageViewSpec {
  ImageViewSpec viewSpec{};
  viewSpec.image = imageHandle;
  viewSpec.format = desc.format;

  if ((desc.usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0u) {
    viewSpec.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    // if (((desc.format & VK_FORMAT_D32_SFLOAT_S8_UINT) != 0u) ||
    //     (desc.format & VK_FORMAT_D24_UNORM_S8_UINT) != 0u) {
    //   viewSpec.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    // }
    viewSpec.viewType = VK_IMAGE_VIEW_TYPE_2D;
  } else {
    viewSpec.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewSpec.viewType =
        (desc.arrayLayers == 1) ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_2D_ARRAY;
  }

  viewSpec.baseMipLevel = 0;
  viewSpec.levelCount = desc.mipLevels;
  viewSpec.baseArrayLayer = 0;
  viewSpec.layerCount = desc.arrayLayers;
  viewSpec.debugName = std::format("{}-View", desc.debugName.value_or("Unnamed Image"));
  return viewSpec;
}

}
