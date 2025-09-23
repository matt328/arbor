#include "resources/images/ImageSystem.hpp"

#include "LogicalImageAllocator.hpp"
#include "LogicalImageViewAllocator.hpp"
#include "bk/Logger.hpp"
#include "ImageManager.hpp"
#include "ImageViewManager.hpp"
#include "SamplerManager.hpp"

#include <vulkan/vulkan_core.h>

namespace arb {

ImageSystem::ImageSystem(Device& device, AllocatorService& allocatorService) {
  Log::trace("Creating ImageSystem");
  imageManager = std::make_unique<ImageManager>(device, allocatorService);
  imageViewManager = std::make_unique<ImageViewManager>(device, *imageManager);
  samplerManager = std::make_unique<SamplerManager>(device);
  logicalImageAllocator = std::make_unique<LogicalImageAllocator>(*imageManager, 3);
  logicalImageViewAllocator = std::make_unique<LogicalImageViewAllocator>(*imageViewManager, 3);
}

ImageSystem::~ImageSystem() {
  Log::trace("Destroying ImageSystem");
}

auto ImageSystem::createImage(const ImageSpec& imageSpec) -> ImageHandle {
  const auto [ici, aci] = fromImageSpec(imageSpec);
  return imageManager->createImage(ici, aci, imageSpec.debugName);
}

auto ImageSystem::createPerFrameImage(const ImageSpec& imageSpec) -> LogicalImageHandle {
  const auto [ici, aci] = fromImageSpec(imageSpec);
  return logicalImageAllocator->createPerFrameImage(ici, aci, imageSpec.debugName);
}

auto ImageSystem::resolveImageHandle(LogicalImageHandle logicalHandle, uint32_t frameIndex)
    -> ImageHandle {
  return logicalImageAllocator->resolve(logicalHandle, frameIndex);
}

auto ImageSystem::getImage(ImageHandle handle) -> const Image& {
  return imageManager->getImage(handle);
}

auto ImageSystem::getImage(LogicalImageHandle handle, uint32_t frameIndex) -> const Image& {
  return getImage(logicalImageAllocator->resolve(handle, frameIndex));
}

auto ImageSystem::destroyImage(LogicalImageHandle handle) -> void {
  logicalImageAllocator->destroyImage(handle);
}

auto ImageSystem::destroyImage(ImageHandle handle) -> void {
  imageManager->destroyImage(handle);
}

auto ImageSystem::createImageView(const ImageViewSpec& imageViewSpec) -> ImageViewHandle {
  const auto [createInfo, imageHandle, name] = fromImageViewSpec(imageViewSpec);
  return imageViewManager->createImageView(createInfo, imageHandle, name);
}

auto ImageSystem::createPerFrameImageView(const ImageViewSpec& imageViewSpec)
    -> LogicalImageViewHandle {
  const auto [ivci, imageHandle, name] = fromImageViewSpec(imageViewSpec);
  return logicalImageViewAllocator->createPerFrameImageView(ivci, imageHandle, name);
}

auto ImageSystem::destroyImageView(ImageViewHandle handle) -> void {
  imageViewManager->destroyImageView(handle);
}

auto ImageSystem::getImageView(ImageViewHandle handle) -> const ImageView& {
  return imageViewManager->getImageView(handle);
}

auto ImageSystem::getImageView(LogicalImageViewHandle logicalHandle, uint32_t frameIndex)
    -> const ImageView& {
  return getImageView(logicalImageViewAllocator->resolve(logicalHandle, frameIndex));
}

auto ImageSystem::createSampler(const SamplerSpec& sci) -> SamplerHandle {
  const auto [createInfo, name] = fromSamplerSpec(sci);
  return samplerManager->createSampler(createInfo, name);
}

auto ImageSystem::destroySampler(SamplerHandle handle) -> void {
  samplerManager->destroySampler(handle);
}

auto ImageSystem::getSampler(SamplerHandle handle) -> const Sampler& {
  return samplerManager->getSampler(handle);
}

auto ImageSystem::fromImageSpec(const ImageSpec& imageSpec)
    -> std::tuple<VkImageCreateInfo, VmaAllocationCreateInfo> {
  const auto imageCreateInfo =
      VkImageCreateInfo{.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                        .imageType = VK_IMAGE_TYPE_2D,
                        .format = imageSpec.format,
                        .extent = VkExtent3D{.width = imageSpec.extent.width,
                                             .height = imageSpec.extent.height,
                                             .depth = 1},
                        .mipLevels = imageSpec.mipLevels,
                        .arrayLayers = imageSpec.layers,
                        .samples = VK_SAMPLE_COUNT_1_BIT,
                        .tiling = VK_IMAGE_TILING_OPTIMAL,
                        .usage = imageSpec.usageFlags,
                        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED};
  constexpr auto imageAllocateCreateInfo =
      VmaAllocationCreateInfo{.usage = VMA_MEMORY_USAGE_GPU_ONLY,
                              .requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT};
  return {imageCreateInfo, imageAllocateCreateInfo};
}

auto ImageSystem::fromImageViewSpec(const ImageViewSpec& imageViewSpec)
    -> std::tuple<VkImageViewCreateInfo, ImageHandle, std::optional<std::string>> {
  VkImageViewCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0;
  info.image = imageManager->getImage(imageViewSpec.image);
  info.viewType = imageViewSpec.viewType;
  info.format = imageViewSpec.format;

  info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

  info.subresourceRange.aspectMask = imageViewSpec.aspectMask;
  info.subresourceRange.baseMipLevel = imageViewSpec.baseMipLevel;
  info.subresourceRange.levelCount = imageViewSpec.levelCount;
  info.subresourceRange.baseArrayLayer = imageViewSpec.baseArrayLayer;
  info.subresourceRange.layerCount = imageViewSpec.layerCount;

  return {info, imageViewSpec.image, imageViewSpec.debugName};
}

auto ImageSystem::fromSamplerSpec(const SamplerSpec& samplerSpec)
    -> std::tuple<VkSamplerCreateInfo, std::optional<std::string>> {
  VkSamplerCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0; // usually 0
  info.magFilter = samplerSpec.magFilter;
  info.minFilter = samplerSpec.minFilter;
  info.mipmapMode = samplerSpec.mipmapMode;
  info.addressModeU = samplerSpec.addressModeU;
  info.addressModeV = samplerSpec.addressModeV;
  info.addressModeW = samplerSpec.addressModeW;
  info.mipLodBias = samplerSpec.mipLodBias;
  info.anisotropyEnable = samplerSpec.anisotropyEnable ? VK_TRUE : VK_FALSE;
  info.maxAnisotropy = samplerSpec.maxAnisotropy;
  info.compareEnable = samplerSpec.compareEnable ? VK_TRUE : VK_FALSE;
  info.compareOp = samplerSpec.compareOp;
  info.minLod = samplerSpec.minLod;
  info.maxLod = samplerSpec.maxLod;
  info.borderColor = samplerSpec.borderColor;
  info.unnormalizedCoordinates = samplerSpec.unnormalizedCoordinates ? VK_TRUE : VK_FALSE;

  return {info, samplerSpec.debugName};
}

}
