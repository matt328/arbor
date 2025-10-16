#pragma once

#include <cstdint>
#include <limits>
#include <glm/glm.hpp>

namespace Arbor::gpu {

constexpr uint32_t INVALID_OFFSET = std::numeric_limits<uint32_t>::max(); // 0xFFFFFFFF

/*
  Each GeometryRegion is a single 'mesh' with multiple vertices. It has offsets into each of the
  deinterleaved buffers of where it's data begins, and the IndexData buffer contains an additional
  offset for each vertex
*/

struct GeometryRegion {
  uint32_t indexCount = 0;
  uint32_t indexOffset = INVALID_OFFSET;

  uint32_t positionOffset = 0;
  uint32_t colorOffset = INVALID_OFFSET;
  uint32_t texCoordOffset = INVALID_OFFSET;
  uint32_t normalOffset = INVALID_OFFSET;
};

// Typical Index Data each index 'indexes' into the GpuVertex*Data buffer
// vec3 pos = pc.posBuf.positions[region.positionOffset + idx];
struct IndexData {
  uint32_t index;
};

struct VertexPositionData {
  glm::vec3 position;
};

struct VertexColorData {
  glm::vec4 color;
};

struct VertexTexCoordData {
  glm::vec2 texCoords;
};

struct VertexNormalData {
  glm::vec3 normal;
};
}
