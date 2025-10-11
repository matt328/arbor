#pragma once

#include "engine/common/SimState.hpp"
#include "engine/common/IStateBuffer.hpp"
#include "engine/common/data/gpu/GeometryRegion.hpp"
#include "engine/common/data/gpu/Object.hpp"
#include "common/HandleMapperTypes.hpp"
#include "geometry/VirtualGeometryAllocator.hpp"

namespace arb {

class Frame;

struct PerFrameUploaderDeps {
  GeometryHandleMapper& geometryHandleMapper;
  TextureHandleMapper& textureHandleMapper;
  VirtualAllocationManager& allocationManager;
};

struct PerFrameUploaderConfig {
  IStateBuffer<SimState>& stateBuffer;
};

class PerFrameUploader {
public:
  PerFrameUploader(const PerFrameUploaderDeps& deps, const PerFrameUploaderConfig& config);
  ~PerFrameUploader();

  /// Resolves opaque handles into resources, and prepares and uploads per-frame data to the GPU.
  /// Per-Frame data includes GpuFrameData(camera matrices, time, maxObjects) GpuResourceTable,
  /// GpuGeometryRegions, Materials, ObjectData, ObjectPositions, ObjectRotations, ObjectScales
  auto upload(Frame* frame) -> void;

private:
  IStateBuffer<SimState>& stateBuffer;

  std::vector<gpu::GeometryRegion> geometryRegionCache;
  std::vector<gpu::Material> materialCache;

  GeometryHandleMapper& geometryHandleMapper;
  TextureHandleMapper& textureHandleMapper;
  VirtualAllocationManager& allocationManager;

  /// Updates geometryRegionCache and materialCache from SimState
  void collectGeometryDetails(Frame* frame, SimState& state);
  void uploadFrameData(Frame* frame, SimState& state, uint32_t objectCount);
  void uploadResourceTable(Frame* frame);
  void uploadGeometryRegion(Frame* frame);
  void uploadMaterials(Frame* frame);
  void uploadObjectData(Frame* frame);
};

}
