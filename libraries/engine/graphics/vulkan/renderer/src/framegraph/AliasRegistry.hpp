#pragma once

#include <string>
#include <unordered_map>
#include <vulkan/vulkan_core.h>

#include "bk/NonCopyMove.hpp"
#include "common/ImageCreateDescription.hpp"
#include "common/ImageLifetime.hpp"
#include "core/Swapchain.hpp"
#include "common/BufferCreateInfo.hpp"
#include "resources/BufferHandle.hpp"
#include "core/Image.hpp"
#include "core/ImageHandle.hpp"
#include "resources/ResourceSystem.hpp"

namespace arb {

class Frame;

struct AliasRegistryDeps {
  ResourceSystem& resourceSystem;
  Swapchain& swapchain;
};

/// Contains mappings of resource Aliases used in the FrameGraph to their (Logical)Handles.
class AliasRegistry : public NonCopyableMovable {
public:
  explicit AliasRegistry(const AliasRegistryDeps& deps);
  ~AliasRegistry() = default;

  void registerImageAlias(const std::string& alias, ImageCreateDescription desc);
  void registerBufferAlias(const std::string& alias, const BufferCreateInfo& spec);
  void registerBufferAlias(const std::string& alias, BufferHandle handle);

  void buildResources(uint32_t frameCount);

  [[nodiscard]] auto getImage(const std::string& alias, Frame* frame) const -> const Image&;
  [[nodiscard]] auto getImageHandle(const std::string& alias, Frame* frame) const -> ImageHandle;
  [[nodiscard]] auto getImageViewHandle(const std::string& alias, Frame* frame) const
      -> ImageViewHandle;
  [[nodiscard]] auto getBufferHandle(const std::string& alias, const Frame* frame) const
      -> BufferHandle;
  [[nodiscard]] auto getBuffer(const std::string& alias, const Frame* frame) const -> Buffer&;

  [[nodiscard]] auto getAttachmentInfo(const std::string& alias,
                                       Frame* frame,
                                       VkImageLayout layout,
                                       VkAttachmentLoadOp loadOp,
                                       VkAttachmentStoreOp storeOp,
                                       std::optional<VkClearValue> clearValue = std::nullopt) const
      -> VkRenderingAttachmentInfo;

  void logAliases() const;

private:
  ResourceSystem& resourceSystem;
  Swapchain& swapchain;

  [[nodiscard]] auto getSwapchainImage(uint32_t index) const -> Image&;
  [[nodiscard]] auto getSwapchainImageHandle(uint32_t index) const -> ImageHandle;

  struct AliasImageEntry {
    ImageLifetime lifetime;
    std::vector<ImageHandle> imageHandles;
  };

  struct AliasImageViewEntry {
    ImageLifetime lifetime;
    std::vector<ImageViewHandle> imageViewHandles;
  };

  std::unordered_map<std::string, ImageCreateDescription> aliasImageSpecMap;
  std::unordered_map<std::string, AliasImageEntry> aliasImageHandleMap;
  std::unordered_map<std::string, AliasImageViewEntry> aliasImageViewMap;

  struct AliasBufferEntry {
    BufferLifetime lifetime;
    std::vector<BufferHandle> bufferHandles;
  };

  std::unordered_map<std::string, BufferCreateInfo> aliasBufferSpecMap;
  std::unordered_map<std::string, AliasBufferEntry> aliasBufferHandleMap;

  static auto createImageViewSpec(ImageHandle imageHandle, const ImageCreateDescription& desc)
      -> ImageViewSpec;
};

}
