#pragma once

#include "core/pipeline/PipelineManager.hpp"
#include "engine/common/RenderSurfaceState.hpp"
#include "framegraph/AliasRegistry.hpp"
#include "framegraph/render-pass/IRenderPass.hpp"
#include "buffers/BufferHandle.hpp"

namespace arb {

struct CullingPassDeps {
  AliasRegistry& aliasRegistry;
  PipelineManager& pipelineManager;
};

struct CullingPassConfig {
  RenderSurfaceState initialState{};
  LogicalBufferHandle resourceTable{};
  LogicalBufferHandle frameData{};
};

class CullingPass : public IRenderPass {

public:
  CullingPass(const CullingPassDeps& deps, const CullingPassConfig& config);
  ~CullingPass();

  [[nodiscard]] auto getId() const -> PassId override;
  auto execute(Frame* frame, VkCommandBuffer cmdBuffer) -> void override;
  [[nodiscard]] auto getDescription() const -> PassDescription override;

  void resize(const RenderSurfaceState& newState) override;

private:
  AliasRegistry& aliasRegistry;
  PipelineManager& pipelineManager;

  PipelineUnitHandle pipelineHandle{};

  LogicalBufferHandle resourceTable;
  LogicalBufferHandle frameData;

  RenderSurfaceState surfaceState{};

  struct PushConstants {
    uint64_t resourceTableAddress;
    uint64_t frameDataAddress;
  };
};

}
