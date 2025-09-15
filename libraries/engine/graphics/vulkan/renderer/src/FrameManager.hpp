#pragma once

#include <memory>
#include <unordered_map>
#include <variant>
#include <vector>

#include "bk/NonCopyMove.hpp"

#include "common/Semaphore.hpp"
#include "common/Fence.hpp"
#include "common/ImageAcquireResult.hpp"
#include "framegraph/ImageAlias.hpp"
#include "framegraph/ResourceAliases.hpp"
#include "framegraph/barriers/LastBufferUse.hpp"
#include "framegraph/barriers/LastImageUse.hpp"

namespace arb {

struct GraphicsOptions;
class Frame;
class Device;
class Swapchain;

class FrameManager : NonCopyableMovable {
public:
  FrameManager(const GraphicsOptions& graphicsOptions, Device& newDevice, Swapchain& swapchain);
  ~FrameManager();

  auto acquireFrame() -> std::variant<Frame*, ImageAcquireResult>;

  auto setSubmittedFrame(uint8_t index, bool newSubmitted) -> void;
  auto isSubmittedFrame(uint8_t index) -> bool;

  auto getFrameInflightFence(uint8_t index) -> Fence&;
  auto getFrameImageAvailableSemaphore(uint8_t index) -> Semaphore&;
  auto getFrameComputeFinishedSemaphore(uint8_t index) -> Semaphore&;

  auto setFrameSwapchainIndex(uint8_t index, uint32_t imageIndex) -> void;
  auto getFrameSwapchainIndex(uint8_t index) -> uint32_t;

  auto setFrameLastImageUse(uint8_t index, ImageAlias alias, LastImageUse use) -> void;
  auto getFrameLastImageUse(uint8_t index, ImageAlias alias) -> LastImageUse;

  auto setFrameLastBufferUse(uint8_t index, BufferAliasVariant alias, LastBufferUse use) -> void;
  auto getFrameLastBufferUse(uint8_t index, BufferAliasVariant alias) -> LastBufferUse;

private:
  Device& device;
  Swapchain& swapchain;
  size_t currentFrame;
  std::vector<std::unique_ptr<Frame>> frames;

  std::vector<bool> submittedFrames;
  std::vector<uint32_t> swapchainImageIndices;
  std::vector<Fence> inFlightFences;
  std::vector<Semaphore> imageAvailableSemaphores;
  std::vector<Semaphore> computeFinishedSemaphores;
  std::vector<std::unordered_map<ImageAlias, LastImageUse>> lastImageUses;
  std::vector<std::unordered_map<BufferAliasVariant, LastBufferUse>> lastBufferUses;
};

}
