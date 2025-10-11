#include "PerFrameUploader.hpp"

#include <Tracy.hpp>

#include "bk/Log.hpp"

#include "Frame.hpp"

namespace arb {

PerFrameUploader::PerFrameUploader(const PerFrameUploaderDeps& deps,
                                   const PerFrameUploaderConfig& config)
    : stateBuffer(config.stateBuffer),
      geometryHandleMapper{deps.geometryHandleMapper},
      textureHandleMapper{deps.textureHandleMapper},
      allocationManager{deps.allocationManager} {
  LOG_TRACE_L1(Log::Renderer, "Constructing PerFrameUploader");
}

PerFrameUploader::~PerFrameUploader() {
  LOG_TRACE_L1(Log::Renderer, "Destroying PerFrameUploader");
}

auto PerFrameUploader::upload(Frame* frame) -> void {
  ZoneScoped;
  auto slot = stateBuffer.checkoutForRead();
  if (slot.has_value()) {
    auto& simState = slot.value()->data;
    collectGeometryDetails(frame, simState);
    stateBuffer.commitRead(slot.value());
  }
}

auto PerFrameUploader::collectGeometryDetails(Frame* frame, SimState& state) -> void {
  const auto objectCount = state.objectMetadata.size();

  geometryRegionCache.clear();
  geometryRegionCache.reserve(objectCount);
  materialCache.clear();

  for (size_t i = 0; i < objectCount; ++i) {
    const auto handles = state.stateHandles[i];
    // Create GeometryRegionData
    auto regionHandle = geometryHandleMapper.toInternal(handles.geometryHandle);
    auto regionData = allocationManager.getRegionData(*regionHandle);
    geometryRegionCache.push_back(regionData);
    state.objectMetadata[i].geometryRegionId = geometryRegionCache.size() - 1;

    // Create MaterialData
    if (state.stateHandles[i].textureHandle) {
      auto textureHandle = textureHandleMapper.toInternal(*state.stateHandles[i].textureHandle);
      uint32_t textureId = 0; // textureArena->getTextureIndex(*textureHandle);
      materialCache.push_back(
          gpu::Material{.baseColor = {1.f, 0.f, 0.f, 1.f}, .albedoTextureId = textureId});
    }
  }
  uploadFrameData(frame, state, objectCount);
  uploadResourceTable(frame);
  uploadGeometryRegion(frame);
  uploadMaterials(frame);
  uploadObjectData(frame);

  frame->setObjectCount(objectCount);
}

void PerFrameUploader::uploadFrameData(Frame* frame, SimState& state, uint32_t objectCount) {
}

void PerFrameUploader::uploadResourceTable(Frame* frame) {
}

void PerFrameUploader::uploadGeometryRegion(Frame* frame) {
}

void PerFrameUploader::uploadMaterials(Frame* frame) {
}

void PerFrameUploader::uploadObjectData(Frame* frame) {
}

}
