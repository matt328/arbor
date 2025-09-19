#pragma once

#include <optional>

#include "LastBufferUse.hpp"
#include "framegraph/barriers/BufferBarrierPrecursor.hpp"
#include "framegraph/barriers/ImageBarrierPrecursor.hpp"
#include "framegraph/barriers/LastImageUse.hpp"

namespace arb::barriers {

static inline auto build(const BufferBarrierPrecursor& bbp, std::optional<LastBufferUse> lastUse)
    -> std::optional<VkBufferMemoryBarrier2> {
  const auto srcStage = lastUse ? lastUse->stageMask : VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
  const auto srcAccess = lastUse ? lastUse->accessMask : VK_ACCESS_2_NONE;

  if (srcStage == bbp.stageFlags && srcAccess == bbp.accessFlags) {
    return std::nullopt;
  }

  return VkBufferMemoryBarrier2{
      .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
      .srcStageMask = srcStage,
      .srcAccessMask = srcAccess,
      .dstStageMask = bbp.stageFlags,
      .dstAccessMask = bbp.accessFlags,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .offset = 0,
      .size = VK_WHOLE_SIZE,
  };
}

static inline auto build(const ImageBarrierPrecursor& ibp, std::optional<LastImageUse> lastUse)
    -> std::optional<VkImageMemoryBarrier2> {
  const auto oldLayout = lastUse ? lastUse->layout : VK_IMAGE_LAYOUT_UNDEFINED;
  const auto srcStage = lastUse ? lastUse->stage : VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
  const auto srcAccess = lastUse ? lastUse->access : VK_ACCESS_2_NONE;

  if (oldLayout == ibp.layout && srcStage == ibp.stageFlags && srcAccess == ibp.accessFlags) {
    return std::nullopt;
  }

  return VkImageMemoryBarrier2{.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                               .srcStageMask = srcStage,
                               .srcAccessMask = srcAccess,
                               .dstStageMask = ibp.stageFlags,
                               .dstAccessMask = ibp.accessFlags,
                               .oldLayout = oldLayout,
                               .newLayout = ibp.layout,
                               .subresourceRange = VkImageSubresourceRange{
                                   .aspectMask = ibp.aspectFlags,
                                   .baseMipLevel = 0,
                                   .levelCount = 1,
                                   .baseArrayLayer = 0,
                                   .layerCount = 1,
                               }};
}

}
