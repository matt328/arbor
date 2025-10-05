#include "PerFrameUploader.hpp"

#include <Tracy.hpp>

#include "bk/Log.hpp"

namespace arb {

PerFrameUploader::PerFrameUploader(IStateBuffer<SimState>& newStateBuffer,
                                   GeometryHandleMapper& newGeometryHandleMapper)
    : stateBuffer(newStateBuffer), geometryHandleMapper{newGeometryHandleMapper} {
  LOG_TRACE_L1(Log::Renderer, "Constructing PerFrameUploader");
}

PerFrameUploader::~PerFrameUploader() {
  LOG_TRACE_L1(Log::Renderer, "Destroying PerFrameUploader");
}

auto PerFrameUploader::upload() -> void {
  ZoneScoped;
  auto slot = stateBuffer.checkoutForRead();
  if (slot.has_value()) {
    auto& simState = slot.value()->data;
    collectGeometryDetails(simState);
    stateBuffer.commitRead(slot.value());
  }
}

auto PerFrameUploader::collectGeometryDetails(SimState& state) -> void {
  state.clear();
  geometryRegionCache.reserve(state.objectMetadata.size());
  materialCache.clear();

  for (size_t i = 0; i < state.objectMetadata.size(); ++i) {
    auto regionHandle = geometryHandleMapper.toInternal(state.stateHandles[i].geometryHandle);
    // auto regionData = geometryAllocator->getRegionData(*regionHandle);

    // geometryRegionCache.push_back(regionData);

    // state.objectMetadata[i].geometryRegionId = geometryRegionCache.size() - 1;

    // if (state.stateHandles[i].textureHandle) {
    //   auto textureHandle = textureHandleMapper->toInternal(*state.stateHandles[i].textureHandle);
    //   auto textureId = textureArena->getTextureIndex(*textureHandle);
    //   materialCache.push_back(
    //       gpu::Material{.baseColor = {1.f, 0.f, 0.f, 1.f}, .albedoTextureId = textureId});
    // }
  }
}

auto PerFrameUploader::uploadPerFrameData(Frame* frame, SimState& state) -> void {
}

}
