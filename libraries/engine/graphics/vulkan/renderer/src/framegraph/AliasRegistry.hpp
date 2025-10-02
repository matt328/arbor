#pragma once

#include <string>
#include <unordered_map>
#include <vulkan/vulkan_core.h>

#include "bk/NonCopyMove.hpp"
#include "common/ImageCreateDescription.hpp"
#include "resources/BufferHandle.hpp"
#include "resources/images/Image.hpp"
#include "resources/images/ImageHandle.hpp"
#include "resources/ResourceSystem.hpp"

namespace arb {

struct BufferSpec {};

/// Contains mappings of resource Aliases used in the FrameGraph to their (Logical)Handles.
class AliasRegistry : public NonCopyableMovable {
public:
  explicit AliasRegistry(ResourceSystem& newResourceSystem);
  ~AliasRegistry() = default;

  void registerImageAlias(const std::string& alias, ImageCreateDescription desc);
  void registerBufferAlias(std::string alias, BufferSpec spec);

  void buildResources(uint32_t frameCount);

  [[nodiscard]] auto getImage(std::string_view alias, uint32_t frameIndex) const -> Image&;
  [[nodiscard]] auto getImageHandle(std::string_view alias, uint32_t frameIndex) const
      -> ImageHandle;
  [[nodiscard]] auto getBufferHandle(std::string_view alias, uint32_t frameIndex) const
      -> BufferHandle;
  [[nodiscard]] auto getBuffer(std::string_view alias, uint32_t frameIndex) const -> Buffer&;

  [[nodiscard]] auto getAttachmentInfo(const std::string& alias,
                                       uint32_t frameIndex,
                                       VkImageLayout layout,
                                       VkAttachmentLoadOp loadOp,
                                       VkAttachmentStoreOp storeOp,
                                       std::optional<VkClearValue> clearValue = std::nullopt) const
      -> VkRenderingAttachmentInfo;

  void logAliases() const;

private:
  ResourceSystem& resourceSystem;

  std::unordered_map<std::string, ImageCreateDescription> aliasImageSpecMap;
  std::unordered_map<std::string, std::vector<ImageViewHandle>> aliasImageViewMap;

  static auto createImageViewSpec(ImageHandle imageHandle, const ImageCreateDescription& desc)
      -> ImageViewSpec;
};

}
