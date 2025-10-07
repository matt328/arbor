#include "AliasRegistry.hpp"

#include <cassert>
#include <cpptrace/cpptrace.hpp>
#include <cpptrace/exceptions.hpp>
#include <vulkan/vulkan_core.h>

#include "bk/Log.hpp"
#include "common/ImageCreateDescription.hpp"
#include "common/ImageLifetime.hpp"
#include "renderer/Constants.hpp"
#include "Frame.hpp"
#include "common/BufferCreateInfo.hpp"

namespace arb {

AliasRegistry::AliasRegistry(const AliasRegistryDeps& deps)
    : resourceSystem{deps.resourceSystem}, swapchain{deps.swapchain} {
  LOG_TRACE_L1(Log::Renderer, "Creating AliasRegistry");
}

void AliasRegistry::registerImageAlias(const std::string& alias, ImageCreateDescription desc) {
  LOG_TRACE_L1(Log::Renderer, "Registering ImageAlias {}", alias);
  if (aliasImageSpecMap.contains(alias)) {
    if (!(aliasImageSpecMap.at(alias) == desc)) {
      throw cpptrace::runtime_error("Alias " + alias + " registered with a different spec");
    }
    LOG_TRACE_L1(Log::Renderer, "Alias already existd: {}", alias);
    return;
  }
  LOG_TRACE_L1(Log::Renderer, "Registering new alias {}", alias);
  aliasImageSpecMap.emplace(alias, desc);
}

void AliasRegistry::registerBufferAlias(const std::string& alias, const BufferCreateInfo& spec) {
  LOG_TRACE_L1(Log::Renderer, "Registering BufferAlias {}", alias);
  if (aliasBufferSpecMap.contains(alias)) {
    if (!(aliasBufferSpecMap.at(alias) == spec)) {
      throw cpptrace::runtime_error("Alias " + alias + " registered with a different spec");
    }
    LOG_TRACE_L1(Log::Renderer, "Alias already existed: {}", alias);
    return;
  }
  LOG_TRACE_L1(Log::Renderer, "Registering new alias {}", alias);
  aliasBufferSpecMap.emplace(alias, spec);
}

void AliasRegistry::registerBufferAlias(const std::string& alias, BufferHandle handle) {
  aliasBufferHandleMap[alias].lifetime = BufferLifetime::Persistent;
  aliasBufferHandleMap[alias].bufferHandles.push_back(handle);
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

  for (const auto& [alias, spec] : aliasBufferSpecMap) {
    uint32_t instances = 1;
    switch (spec.bufferLifetime) {
      case BufferLifetime::Persistent:
        instances = 1;
        break;
      case BufferLifetime::Transient:
        instances = frameCount;
        break;
    }
    aliasBufferHandleMap[alias].bufferHandles.resize(instances);
    for (uint32_t i = 0; i < instances; ++i) {
      const auto bufferHandle = resourceSystem.createBuffer(spec);
      aliasBufferHandleMap[alias].lifetime = spec.bufferLifetime;
      aliasBufferHandleMap[alias].bufferHandles[i] = bufferHandle;
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

auto AliasRegistry::getImageViewHandle(const std::string& alias, Frame* frame) const
    -> ImageViewHandle {
  if (aliasImageViewMap.contains(alias)) {
    const auto& entry = aliasImageViewMap.at(alias);
    switch (entry.lifetime) {
      case ImageLifetime::Persistent:
        return entry.imageViewHandles[0];
      case ImageLifetime::Transient:
      case ImageLifetime::Swapchain:
        assert(frame->getIndex() < entry.imageViewHandles.size());
        return entry.imageViewHandles[frame->getIndex()];
    }
    throw cpptrace::logic_error("Unhandled ImageLifetime");
  }

  throw cpptrace::logic_error(std::format("Requested alias {} not registered", alias));
}

auto AliasRegistry::getBufferHandle(const std::string& alias, const Frame* frame) const
    -> BufferHandle {
  if (aliasBufferHandleMap.contains(alias)) {
    const auto& entry = aliasBufferHandleMap.at(alias);
    switch (entry.lifetime) {
      case BufferLifetime::Persistent:
        return entry.bufferHandles.front();
      case arb::BufferLifetime::Transient:
        assert(frame->getIndex() < entry.bufferHandles.size());
        return entry.bufferHandles[frame->getIndex()];
    }
  }
  LOG_ERROR(Log::Core, "throwing cpptrace");
  // throw cpptrace::logic_error(std::format("Requested alias has not been registered: {}", alias));
  return BufferHandle{};
}

auto AliasRegistry::getBuffer(const std::string& alias, const Frame* frame) const -> Buffer& {
  const auto bufferHandle = getBufferHandle(alias, frame);
  return resourceSystem.getBuffer(bufferHandle);
}

auto AliasRegistry::getAttachmentInfo(const std::string& alias,
                                      Frame* frame,
                                      VkImageLayout layout,
                                      VkAttachmentLoadOp loadOp,
                                      VkAttachmentStoreOp storeOp,
                                      std::optional<VkClearValue> clearValue) const
    -> VkRenderingAttachmentInfo {

  auto getView = [&]() -> const ImageView& {
    if (alias == Constants::SwapchainAlias) {
      return swapchain.getImageView(frame->getSwapchainImageIndex());
    }
    const auto& imageViewHandle = getImageViewHandle(alias, frame);
    return resourceSystem.getImageView(imageViewHandle);
  };

  const auto& vkView = getView();
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
