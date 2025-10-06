#pragma once

#include <memory>
#include <vector>

#include <bk/Log.hpp>
#include "BufferBarrierPrecursor.hpp"
#include "framegraph/barriers/BarrierPrecursorPlan.hpp"
#include "framegraph/render-pass/IRenderPass.hpp"

namespace arb {

static inline auto isWriteAccess(VkAccessFlags2 flags) -> bool {
  constexpr VkAccessFlags2 writeFlags =
      VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
      VK_ACCESS_2_TRANSFER_WRITE_BIT | VK_ACCESS_2_SHADER_WRITE_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT;

  return (flags & writeFlags) != 0;
}

static inline auto generatePrecursorPlan(const std::vector<std::unique_ptr<IRenderPass>>& passes)
    -> BarrierPrecursorPlan {
  BarrierPrecursorPlan result{};
  for (const auto& pass : passes) {
    const auto& passId = pass->getId();
    const auto desc = pass->getDescription();
    for (const auto& imageReq : desc.images) {
      result.imagePrecursors[passId].push_back(
          {.alias = imageReq.alias,
           .isWriteAccess = isWriteAccess(imageReq.useDesc.accessFlags),
           .accessFlags = imageReq.useDesc.accessFlags,
           .stageFlags = imageReq.useDesc.stageFlags,
           .layout = imageReq.useDesc.imageLayout,
           .aspectFlags = imageReq.useDesc.aspectFlags});
    }
    for (const auto& bufferReq : desc.buffers) {
      result.bufferPrecursors[passId].push_back(
          BufferBarrierPrecursor{.alias = bufferReq.alias,
                                 .accessFlags = bufferReq.useDesc.accessFlags,
                                 .stageFlags = bufferReq.useDesc.stageFlags});
    }

    // LOG_TRACE_L1(Log::Renderer, "Generated BarrierPrecursorPlan:\n{}", result);
  }
  return result;
}

}
