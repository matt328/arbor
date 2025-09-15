#include "FrameGraph.hpp"

#include <cassert>

#include "Frame.hpp"
#include "barriers/PrecursorGenerator.hpp"
#include "bk/Logger.hpp"
#include "core/command-buffers/CommandBuffer.hpp"
#include "core/command-buffers/CommandBufferManager.hpp"

#include "resources/ResourceFacade.hpp"
#include "framegraph/passes/IRenderPass.hpp"

#include "framegraph/AliasRegistry.hpp"

namespace arb {

FrameGraph::FrameGraph(CommandBufferManager& newCommandBufferManager,
                       ResourceFacade& newResourceFacade,
                       AliasRegistry& newAliasRegistry)
    : commandBufferManager{newCommandBufferManager},
      resourceFacade{newResourceFacade},
      aliasRegistry{newAliasRegistry} {
}

FrameGraph::~FrameGraph() {
  Log::trace("Destroying FrameGraph");
}

auto FrameGraph::addPass(std::unique_ptr<IRenderPass>&& pass) -> void {
  const auto size = renderPasses.size();
  const auto passId = pass->getId();
  renderPasses.push_back(std::move(pass));
  passesById.emplace(passId, size);
}

auto FrameGraph::getPass(PassId id) -> std::unique_ptr<IRenderPass>& {
  assert(passesById.contains(id));
  return renderPasses[passesById.at(id)];
}

auto FrameGraph::bake() -> void {
  barrierPrecursorPlan = generatePrecursorPlan(renderPasses);
}

auto FrameGraph::execute(Frame* frame) -> FrameGraphResult {
  auto result = FrameGraphResult{};

  for (const auto& renderPass : renderPasses) {
    const auto passId = renderPass->getId();
    auto imageBarriers = std::vector<VkImageMemoryBarrier2>{};

    if (barrierPrecursorPlan.imagePrecursors.contains(passId)) {
      for (const auto& precursor : barrierPrecursorPlan.imagePrecursors.at(passId)) {
        const auto handle = resourceFacade.getImageHandle(aliasRegistry.getHandle(precursor.alias),
                                                          frame->getIndex());
        auto lastUse = std::optional<LastImageUse>();
        if (precursor.alias == ImageAlias::SwapchainImage) {
          lastUse = getSwapchainImageLastUse(handle);
        } else {
          lastUse = frame->getLastImageUse(precursor.alias);
        }
        auto imageBarrier = BarrierBuilder::build(precursor, lastUse);
        if (imageBarrier) {
          const auto& image = resourceFacade.getImage(handle);
          imageBarrier->setImage(image);
          imageBarriers.push_back(*imageBarrier);
        }
        const auto newLastUse = LastImageUse{
            .accessMode = precursor.accessMode,
            .access = precursor.accessFlags,
            .stage = precursor.stageFlags,
            .layout = precursor.layout,
        };
        if (precursor.alias == ImageAlias::SwapchainImage) {
          setSwapchainImageLastUse(handle, newLastUse);
        } else {
          frame->setLastImageUse(precursor.alias, newLastUse);
        }
      }
    }

    auto bufferBarriers = std::vector<VkBufferMemoryBarrier2>{};
    if (barrierPrecursorPlan.bufferPrecursors.contains(passId)) {

      const auto visitor = [&]<typename T>(T&& arg) -> VkBuffer {
        using U = std::decay_t<T>;
        if constexpr (std::is_same_v<U, BufferAlias>) {
          const auto logicalHandle = aliasRegistry.getHandle(std::forward<T>(arg));
          return resourceFacade.getBuffer(logicalHandle, frame->getIndex());
        }
        if constexpr (std::is_same_v<U, GlobalBufferAlias>) {
          const auto handle = aliasRegistry.getHandle(std::forward<T>(arg));
          return resourceFacade.getBuffer(handle);
        }
      };

      for (const auto& precursor : barrierPrecursorPlan.bufferPrecursors.at(passId)) {
        const auto& buffer = std::visit(visitor, precursor.alias);
        auto lastUse = frame->getLastBufferUse(precursor.alias);
        auto bufferBarrier = BarrierBuilder::build(precursor, lastUse);
        if (bufferBarrier) {
          bufferBarrier->setBuffer(*buffer);
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
    commandBuffer.begin(VkCommandBufferBeginInfo{});
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
