#pragma once

#include <optional>

#include "buffers/BufferRegion.hpp"
#include "buffers/ResizeRequest.hpp"
#include "engine/common/HandleGenerator.hpp"
#include "buffers/BufferHandle.hpp"
#include "buffers/GeometryData.hpp"
#include "engine/common/data/gpu/GeometryRegion.hpp"
#include "buffers/GeometryHandle.hpp"

namespace arb {

class GeometryStream;

/// Describes several Regions of buffers in the GeometryBufferPack in terms of the allocations
/// tracked on the host.
struct GeometryRegion {
  uint32_t indexCount{};
  BufferRegion indexRegion;
  BufferRegion positionRegion;
  std::optional<BufferRegion> normalRegion;
  std::optional<BufferRegion> texCoordRegion;
  std::optional<BufferRegion> colorRegion;
  std::optional<BufferRegion> animationDataRegion;
};

/// Describes Allocated Data to be uploaded, contains the data itself, it's destination buffer, it's
/// size, and the offsets in the staging buffer as well as its destination buffer
struct BufferAllocation {
  size_t dataSize{};
  std::shared_ptr<std::vector<std::byte>> data = nullptr;
  BufferHandle dstBuffer{};
  size_t stagingOffset{};
  size_t dstOffset{};
};

/// Holds the GeometryAllocator region's handle as well as the BufferAllocation matching said region
struct GeometryAllocation {
  GeometryRegionHandle regionHandle;
  std::vector<BufferAllocation> bufferAllocations;
};

class VirtualAllocationManager {
public:
  explicit VirtualAllocationManager(GeometryStream& newGeometryStream);
  ~VirtualAllocationManager();

  auto checkSizes(const GeometryData& data, size_t size) -> std::vector<ResizeRequest>;

  auto buildAllocationPlan(const GeometryData& data) -> GeometryAllocation;

  [[nodiscard]] auto getRegionData(GeometryRegionHandle handle) const -> gpu::GeometryRegion;

private:
  GeometryStream& geometryStream;

  HandleGenerator<GeometryRegionTag> regionGenerator;
  std::unordered_map<GeometryRegionHandle, gpu::GeometryRegion> regionTable;
};

}
