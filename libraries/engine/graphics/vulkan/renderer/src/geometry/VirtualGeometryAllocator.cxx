#include "VirtualGeometryAllocator.hpp"

#include "bk/Logger.hpp"
#include "geometry/GeometryStream.hpp"

namespace arb {

VirtualAllocationManager::VirtualAllocationManager(GeometryStream& newGeometryStream)
    : geometryStream{newGeometryStream} {
  Log::trace("Creating VirtualAllocationManager");
}

VirtualAllocationManager::~VirtualAllocationManager() {
  Log::trace("Destroying VirtualAllocationManager");
}

auto VirtualAllocationManager::checkSizes(const GeometryData& data, size_t size)
    -> std::vector<ResizeRequest> {
  return geometryStream.checkSizes(data);
}

auto VirtualAllocationManager::buildAllocationPlan(const GeometryData& data) -> GeometryAllocation {
  auto geometryRegion = GeometryRegion{};
  auto uploadList = std::vector<BufferAllocation>{};

  // {
  //   const auto size = data.indexData->size();
  //   const auto stagingRegion =
  //       transferContext.stagingAllocator->allocate(BufferRequest{.size = size});

  //   geometryRegion.indexRegion = geometryStream.allocateIndexBuffer(BufferRequest{.size = size});

  //   geometryRegion.indexCount = data.indexData->size() / sizeof(gpu::IndexData);

  //   uploadList.push_back({
  //       .dataSize = size,
  //       .data = data.indexData,
  //       .dstBuffer = geometryStream.getIndexBuffer(),
  //       .stagingOffset = stagingRegion.offset,
  //       .dstOffset = geometryRegion.indexRegion.offset,
  //   });
  // }

  // {
  //   auto size = data.positionData->size();
  //   auto stagingRegion = transferContext.stagingAllocator->allocate(BufferRequest{.size = size});
  //   geometryRegion.positionRegion =
  //       geometryStream.allocatePositionBuffer(BufferRequest{.size = size});
  //   uploadList.push_back({
  //       .dataSize = size,
  //       .data = data.positionData,
  //       .dstBuffer = geometryStream.getPositionBuffer(),
  //       .stagingOffset = stagingRegion.offset,
  //       .dstOffset = geometryRegion.positionRegion.offset,
  //   });
  // }

  // if (data.colorData != nullptr) {
  //   auto size = data.colorData->size();
  //   auto stagingRegion = transferContext.stagingAllocator->allocate(BufferRequest{.size = size});
  //   geometryRegion.colorRegion = geometryStream.allocateColorBuffer(BufferRequest{.size = size});
  //   uploadList.push_back({
  //       .dataSize = size,
  //       .data = data.colorData,
  //       .dstBuffer = geometryStream.getColorBuffer(),
  //       .stagingOffset = stagingRegion.offset,
  //       .dstOffset = geometryRegion.colorRegion->offset,
  //   });
  // }

  // if (data.texCoordData != nullptr) {
  //   auto size = data.texCoordData->size();
  //   auto stagingRegion = transferContext.stagingAllocator->allocate(BufferRequest{.size = size});
  //   geometryRegion.texCoordRegion =
  //       geometryStream.allocateTexCoordBuffer(BufferRequest{.size = size});
  //   uploadList.push_back({
  //       .dataSize = size,
  //       .data = data.texCoordData,
  //       .dstBuffer = geometryStream.getTexCoordBuffer(),
  //       .stagingOffset = stagingRegion.offset,
  //       .dstOffset = geometryRegion.texCoordRegion->offset,
  //   });
  // }

  const auto handle = regionGenerator.requestHandle();
  // regionTable.try_emplace(handle, geometryRegion);
  return {.regionHandle = handle, .bufferAllocations = uploadList};
}

}
