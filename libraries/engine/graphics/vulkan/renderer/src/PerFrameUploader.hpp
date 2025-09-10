#pragma once

#include "engine/common/SimState.hpp"
#include "engine/common/IStateBuffer.hpp"
#include "engine/common/data/gpu/GeometryRegion.hpp"
#include "engine/common/data/gpu/Object.hpp"
#include "common/HandleMapperTypes.hpp"

namespace arb {

class Frame;

class PerFrameUploader {
public:
  PerFrameUploader(IStateBuffer<SimState>& newStateBuffer,
                   GeometryHandleMapper& newGeometryHandleMapper);
  ~PerFrameUploader();

  /// Resolves opaque handles into resources, and prepares and uploads per-frame data to the GPU.
  /// Per-Frame data includes GpuFrameData(camera matrices, time, maxObjects) GpuResourceTable,
  /// GpuGeometryRegions, Materials, ObjectData, ObjectPositions, ObjectRotations, ObjectScales
  auto upload() -> void;

private:
  IStateBuffer<SimState>& stateBuffer;

  std::vector<gpu::GeometryRegion> geometryRegionCache;
  std::vector<gpu::Material> materialCache;

  GeometryHandleMapper& geometryHandleMapper;

  /// Updates geometryRegionCache and materialCache from SimState
  auto collectGeometryDetails(SimState& state) -> void;
  auto uploadPerFrameData(Frame* frame, SimState& state) -> void;
};

}
