#include "PerFrameUploader.hpp"

#include "bk/Logger.hpp"

namespace arb {

PerFrameUploader::PerFrameUploader(IStateBuffer<SimState>& newStateBuffer)
    : stateBuffer(newStateBuffer) {
  Log::trace("Constructing PerFrameUploader");
}

PerFrameUploader::~PerFrameUploader() {
  Log::trace("Destroying PerFrameUploader");
}

auto PerFrameUploader::upload() -> void {
  auto slot = stateBuffer.checkoutForRead();
  auto& simState = slot.value()->data;

  collectGeometryDetails(simState);
}

auto PerFrameUploader::collectGeometryDetails(SimState& state) -> void {
  state.clear();
  geometryRegionCache.reserve(state.objectMetadata.size());
  materialCache.clear();

  for (size_t i = 0; i < state.objectMetadata.size(); ++i) {
    // auto regionHandle = geometryHandleMapper->toInternal(state.stateHandles[i].geometryHandle);
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

}
