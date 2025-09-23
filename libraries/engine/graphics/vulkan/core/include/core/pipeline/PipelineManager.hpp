#pragma once

#include "PipelineLayout.hpp"
#include "Pipeline.hpp"
#include "PipelineUnitCreateInfo.hpp"
#include "core/Device.hpp"
#include "bk/NonCopyMove.hpp"
#include "core/pipeline/Pipeline.hpp"
#include "engine/common/Handle.hpp"
#include "engine/common/HandleGenerator.hpp"

namespace arb {

struct PipelineUnit {
  Pipeline pipeline;
  PipelineLayout layout;
};

struct PipelineUnitTag {};
using PipelineUnitHandle = Handle<PipelineUnitTag>;

class PipelineManager : NonCopyableMovable {
public:
  explicit PipelineManager(Device& newDevice);
  ~PipelineManager();

  auto createPipeline(const PipelineCreateInfo& createInfo) -> PipelineUnitHandle;

  [[nodiscard]] auto getPipelineUnit(PipelineUnitHandle handle) const -> const PipelineUnit&;

private:
  Device& device;

  HandleGenerator<PipelineUnitTag> handleGenerator;
  std::unordered_map<PipelineUnitHandle, std::unique_ptr<PipelineUnit>> pipelineMap;

  auto createGraphicsPipeline(const PipelineCreateInfo& createInfo)
      -> std::unique_ptr<PipelineUnit>;
  auto createComputePipeline(const PipelineCreateInfo& createInfo) -> std::unique_ptr<PipelineUnit>;
};

}
