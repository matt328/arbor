#include "FrameGraph.hpp"

#include <cassert>

#include "Frame.hpp"
#include "barriers/PrecursorGenerator.hpp"
#include "bk/Logger.hpp"
#include "core/command-buffers/CommandBuffer.hpp"
#include "core/command-buffers/CommandBufferManager.hpp"

#include "render-pass/pass/ForwardPass.hpp"
#include "framegraph/render-pass/IRenderPass.hpp"

#include "framegraph/AliasRegistry.hpp"
#include "framegraph/barriers/Builders.hpp"
#include "common/ImageCreateDescription.hpp"
#include "vulkan/vulkan_core.h"

namespace arb {

FrameGraph::FrameGraph(const FrameGraphDeps& deps, const FrameGraphConfig& config)
    : commandBufferManager{deps.commandBufferManager},
      pipelineManager{deps.pipelineManager},
      aliasRegistry{deps.aliasRegistry} {
  Log::trace("Creating FrameGraph");

  auto forwardPass = std::make_unique<ForwardPass>(
      ForwardPassDeps{.aliasRegistry = aliasRegistry, .pipelineManager = pipelineManager},
      ForwardPassConfig{.initialSurfaceState = config.initialSurfaceState});

  renderPasses.push_back(std::move(forwardPass));

  compileResources();
  bake();
}

FrameGraph::~FrameGraph() {
  Log::trace("Destroying FrameGraph");
}

void FrameGraph::resize(const RenderSurfaceState& newState) {
}

auto FrameGraph::bake() -> void {
  barrierPrecursorPlan = generatePrecursorPlan(renderPasses);
}

void FrameGraph::compileResources() {
  for (const auto& pass : renderPasses) {
    const auto& desc = pass->getDescription();
    for (const auto& ir : desc.images) {
      if (ir.createDesc) {
        aliasRegistry.registerImageAlias(ir.alias, *ir.createDesc);
      }
    }
  }

  aliasRegistry.buildResources(3);
}

auto FrameGraph::execute(Frame* frame) -> FrameGraphResult {
  auto result = FrameGraphResult{};

  for (const auto& renderPass : renderPasses) {
    const auto passId = renderPass->getId();
    auto imageBarriers = std::vector<VkImageMemoryBarrier2>{};

    if (barrierPrecursorPlan.imagePrecursors.contains(passId)) {
      for (const auto& precursor : barrierPrecursorPlan.imagePrecursors.at(passId)) {
        const auto handle = aliasRegistry.getImageHandle(precursor.alias, frame->getIndex());
        auto lastUse = std::optional<LastImageUse>();
        if (precursor.alias == "SwapchainImage") {
          lastUse = getSwapchainImageLastUse(handle);
        } else {
          lastUse = frame->getLastImageUse(precursor.alias);
        }
        auto imageBarrier = barriers::build(precursor, lastUse);
        if (imageBarrier) {
          const auto& image = aliasRegistry.getImage(precursor.alias, frame->getIndex());
          imageBarrier->image = image;
          imageBarriers.push_back(*imageBarrier);
        }
        const auto newLastUse = LastImageUse{
            .isWriteAccess = precursor.isWriteAccess,
            .access = precursor.accessFlags,
            .stage = precursor.stageFlags,
            .layout = precursor.layout,
        };
        if (precursor.alias == "SwapchainImage") {
          setSwapchainImageLastUse(handle, newLastUse);
        } else {
          frame->setLastImageUse(precursor.alias, newLastUse);
        }
      }
    }

    auto bufferBarriers = std::vector<VkBufferMemoryBarrier2>{};
    if (barrierPrecursorPlan.bufferPrecursors.contains(passId)) {

      for (const auto& precursor : barrierPrecursorPlan.bufferPrecursors.at(passId)) {
        const auto& buffer = aliasRegistry.getBuffer(precursor.alias, frame->getIndex());
        auto lastUse = frame->getLastBufferUse(precursor.alias);
        auto bufferBarrier = barriers::build(precursor, lastUse);
        if (bufferBarrier) {
          bufferBarrier->buffer = buffer;
          bufferBarriers.push_back(*bufferBarrier);
        }
        frame->setLastBufferUse(precursor.alias,
                                LastBufferUse{
                                    .passId = passId,
                                    .accessMask = precursor.accessFlags,
                                    .stageMask = precursor.stageFlags,
                                });
      }
    }

    auto dependencyInfo = VkDependencyInfo{
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .bufferMemoryBarrierCount = static_cast<uint32_t>(bufferBarriers.size()),
        .pBufferMemoryBarriers = bufferBarriers.data(),
        .imageMemoryBarrierCount = static_cast<uint32_t>(imageBarriers.size()),
        .pImageMemoryBarriers = imageBarriers.data(),
    };

    const auto request = CommandBufferRequest{.threadId = std::this_thread::get_id(),
                                              .frameId = frame->getIndex(),
                                              .hashKey = std::hash<PassId>{}(passId),
                                              .queueType = QueueType::Graphics};
    auto& commandBuffer = commandBufferManager.requestCommandBuffer(request);
    commandBuffer.begin(
        VkCommandBufferBeginInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO});
    commandBuffer.pipelineBarrier2(dependencyInfo);
    renderPass->execute(frame, commandBuffer);

    commandBuffer.end();
    result.commandBuffers.push_back(commandBuffer);
  }

  return result;
}

auto FrameGraph::getSwapchainImageLastUse(ImageHandle handle) -> std::optional<LastImageUse> {
  return swapchainLastUses.contains(handle)
             ? std::make_optional<LastImageUse>(swapchainLastUses.at(handle))
             : std::nullopt;
}

auto FrameGraph::setSwapchainImageLastUse(ImageHandle handle, LastImageUse lastImageUse) -> void {
  swapchainLastUses.insert_or_assign(handle, lastImageUse);
}

auto FrameGraph::resetSwapchainUses() -> void {
  swapchainLastUses.clear();
}
}
