#pragma once

#include "bk/IEventQueue.hpp"
#include "bk/NativeWindowHandle.hpp"
#include "bk/NonCopyMove.hpp"

#include "common/Semaphore.hpp"
#include "engine/common/EngineOptions.hpp"

namespace arb {

class Device;
class Swapchain;
class Instance;
class Surface;
class PhysicalDevice;
class AllocatorService;
class CommandBufferManager;
class PipelineManager;

class CoreContext : NonCopyableMovable {
public:
  CoreContext(std::shared_ptr<bk::IEventQueue> newEventQueue,
              const EngineOptions& newOptions,
              bk::NativeWindowHandle newWindowHandle);
  ~CoreContext();

  [[nodiscard]] auto getDevice() const -> Device& {
    return *device;
  }

  [[nodiscard]] auto getSwapchain() const -> Swapchain& {
    return *swapchain;
  }

  [[nodiscard]] auto getAllocatorService() const -> AllocatorService& {
    return *allocatorService;
  }

  [[nodiscard]] auto getTransferSemaphore() const -> Semaphore& {
    return *transferSemaphore;
  }

  [[nodiscard]] auto getGraphicsSemaphore() const -> Semaphore& {
    return *graphicsSemaphore;
  }

  [[nodiscard]] auto getComputeSemaphore() const -> Semaphore& {
    return *computeSemaphore;
  }

  [[nodiscard]] auto getCommandBufferManager() const -> CommandBufferManager& {
    return *commandBufferManager;
  }

  [[nodiscard]] auto getPipelineManager() const -> PipelineManager& {
    return *pipelineManager;
  }

private:
  EngineOptions options;
  std::unique_ptr<Instance> vulkanInstance;
  std::unique_ptr<Surface> surface;
  std::unique_ptr<PhysicalDevice> physicalDevice;
  std::shared_ptr<Device> device;
  std::unique_ptr<Swapchain> swapchain;
  std::unique_ptr<AllocatorService> allocatorService;
  std::unique_ptr<CommandBufferManager> commandBufferManager;
  std::unique_ptr<PipelineManager> pipelineManager;

  std::unique_ptr<Semaphore> transferSemaphore;
  std::unique_ptr<Semaphore> graphicsSemaphore;
  std::unique_ptr<Semaphore> computeSemaphore;

  std::shared_ptr<bk::IEventQueue> eventQueue;
};

}
