#pragma once

#include <memory>
#include <unordered_map>
#include <variant>
#include <vector>

#include "ImageTransitionQueue.hpp"
#include "bk/NonCopyMove.hpp"

#include "common/Semaphore.hpp"
#include "common/Fence.hpp"
#include "common/ImageAcquireResult.hpp"
#include "framegraph/barriers/LastBufferUse.hpp"
#include "framegraph/barriers/LastImageUse.hpp"

namespace arb {

struct EngineOptions;
class Frame;
class Device;
class Swapchain;

class FrameManager : NonCopyableMovable {
public:
  FrameManager(const EngineOptions& options, Device& newDevice, Swapchain& swapchain);
  ~FrameManager();

  auto acquireFrame() -> std::variant<Frame*, ImageAcquireResult>;

  auto setSubmittedFrame(uint8_t index, bool newSubmitted) -> void;
  auto isSubmittedFrame(uint8_t index) -> bool;

  auto getFrameInflightFence(uint8_t index) -> Fence&;
  auto getFrameImageAvailableSemaphore(uint8_t index) -> Semaphore&;
  auto getFrameComputeFinishedSemaphore(uint8_t index) -> Semaphore&;

  auto setFrameSwapchainIndex(uint8_t index, uint32_t imageIndex) -> void;
  auto getFrameSwapchainIndex(uint8_t index) -> uint32_t;

  auto setFrameLastImageUse(uint8_t index, const std::string& alias, LastImageUse use) -> void;
  auto getFrameLastImageUse(uint8_t index, const std::string& alias) -> LastImageUse;

  auto setFrameLastBufferUse(uint8_t index, const std::string& alias, LastBufferUse use) -> void;
  auto getFrameLastBufferUse(uint8_t index, const std::string& alias) -> LastBufferUse;

  auto setImageTransitionInfo(uint8_t index, const std::vector<ImageTransitionInfo>& transitionInfo)
      -> void;
  auto getImageTransitionInfo(uint8_t index) -> std::vector<ImageTransitionInfo>;

private:
  static constexpr uint8_t FramesInFlight = 3;

  Device& device;
  Swapchain& swapchain;
  size_t currentFrame;
  std::vector<std::unique_ptr<Frame>> frames;

  std::vector<bool> submittedFrames;
  std::vector<uint32_t> swapchainImageIndices;
  std::vector<Fence> inFlightFences;
  std::vector<Semaphore> imageAvailableSemaphores;
  std::vector<Semaphore> computeFinishedSemaphores;
  std::vector<std::unordered_map<std::string, LastImageUse>> lastImageUses;
  std::vector<std::unordered_map<std::string, LastBufferUse>> lastBufferUses;
  std::vector<std::vector<ImageTransitionInfo>> imageTransitionInfo;
};

}
