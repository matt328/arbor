#pragma once

#include "core/IResizable.hpp"
#include "engine/common/RenderSurfaceState.hpp"
#include "bk/NonCopyMove.hpp"

#include <expected>
#include <functional>
#include <vulkan/vulkan_core.h>

namespace arb {

class Device;
class Swapchain;
class FrameManager;
class PerFrameUploader;
class AliasRegistry;
class FrameGraph;
class Frame;
struct FrameGraphResult;

struct FrameRendererDeps {
  Device& device;
  Swapchain& swapchain;
  FrameManager& frameManager;
  PerFrameUploader& perFrameUploader;
  AliasRegistry& aliasRegistry;
  FrameGraph& frameGraph;
};

struct FrameRendererCreateInfo {
  RenderSurfaceState renderSurfaceState;
};

enum class FrameStatus : uint8_t {
  Success,
  SwapchainOutOfDate,
  SwapchainSuboptimal,
  AcquireFailed,
  PresentFailed,
  NeedsResize,
  NoFrame
};

class FrameRenderer : public NonCopyableMovable {
public:
  explicit FrameRenderer(const FrameRendererDeps& deps, const FrameRendererCreateInfo& info);
  ~FrameRenderer();

  void renderNextFrame();

  void beginResize();
  void commitResize();
  [[nodiscard]] auto resizeInProgress() const noexcept -> bool;

  void setOnSwapchainResized(std::function<void(RenderSurfaceState)> cb);

private:
  Device& device;
  Swapchain& swapchain;
  FrameManager& frameManager;
  PerFrameUploader& perFrameUploader;
  AliasRegistry& aliasRegistry;
  FrameGraph& frameGraph;

  bool resizePending{false};
  std::function<void(RenderSurfaceState)> onResize;

  auto tryAcquireFrame() -> std::expected<Frame*, FrameStatus>;
  void submitFrame(Frame* frame, const FrameGraphResult& frameResult);
  auto presentFrame(Frame* frame) -> FrameStatus;
};

}
