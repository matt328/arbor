#pragma once

#include <memory>
#include <vulkan/vulkan_core.h>

#include "ImageSpec.hpp"
#include "ImageViewSpec.hpp"
#include "SamplerSpec.hpp"
#include "ImageView.hpp"
#include "core/AllocatorService.hpp"
#include "core/Device.hpp"
#include "resources/images/Image.hpp"
#include "resources/images/ImageHandle.hpp"
#include "resources/images/Sampler.hpp"

namespace arb {

class ImageManager;
class ImageViewManager;
class SamplerManager;
class LogicalImageAllocator;
class LogicalImageViewAllocator;

class ImageSystem {
public:
  ImageSystem(Device& device, AllocatorService& allocatorService);
  ~ImageSystem();

  auto createImage(const ImageSpec& imageSpec) -> ImageHandle;
  auto createPerFrameImage(const ImageSpec& imageSpec) -> LogicalImageHandle;
  auto resolveImageHandle(LogicalImageHandle logicalHandle, uint32_t frameIndex) -> ImageHandle;
  auto getImage(ImageHandle handle) -> const Image&;
  auto getImage(LogicalImageHandle handle, uint32_t frameIndex) -> const Image&;
  auto destroyImage(ImageHandle handle) -> void;
  auto destroyImage(LogicalImageHandle handle) -> void;

  auto createImageView(const ImageViewSpec& imageViewSpec) -> ImageViewHandle;
  auto createPerFrameImageView(const ImageViewSpec& imageViewSpec) -> LogicalImageViewHandle;
  auto resolveImageViewHandle(LogicalImageViewHandle logicalHandle, uint32_t frameIndex)
      -> ImageViewHandle;
  auto destroyImageView(ImageViewHandle handle) -> void;
  auto getImageView(ImageViewHandle handle) -> const ImageView&;
  auto getImageView(LogicalImageViewHandle logicalHandle, uint32_t frameIndex) -> const ImageView&;

  auto createSampler(const SamplerSpec& sci) -> SamplerHandle;
  auto getSampler(SamplerHandle handle) -> const Sampler&;
  auto destroySampler(SamplerHandle) -> void;

private:
  std::unique_ptr<ImageManager> imageManager;
  std::unique_ptr<LogicalImageAllocator> logicalImageAllocator;
  std::unique_ptr<ImageViewManager> imageViewManager;
  std::unique_ptr<LogicalImageViewAllocator> logicalImageViewAllocator;
  std::unique_ptr<SamplerManager> samplerManager;

  static auto fromImageSpec(const ImageSpec& imageSpec)
      -> std::tuple<VkImageCreateInfo, VmaAllocationCreateInfo>;

  auto fromImageViewSpec(const ImageViewSpec& imageViewSpec)
      -> std::tuple<VkImageViewCreateInfo, ImageHandle, std::optional<std::string>>;

  static auto fromSamplerSpec(const SamplerSpec& samplerSpec)
      -> std::tuple<VkSamplerCreateInfo, std::optional<std::string>>;
};

}
