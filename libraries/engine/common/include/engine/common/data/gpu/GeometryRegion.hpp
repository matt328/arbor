#pragma once

namespace arb::gpu {

constexpr uint32_t INVALID_OFFSET = std::numeric_limits<uint32_t>::max(); // 0xFFFFFFFF

struct GeometryRegion {
  uint32_t indexCount = 0;
  uint32_t indexOffset = INVALID_OFFSET;

  uint32_t positionOffset = 0;
  uint32_t colorOffset = INVALID_OFFSET;
  uint32_t texCoordOffset = INVALID_OFFSET;
  uint32_t normalOffset = INVALID_OFFSET;
};

// Typical Index Data each index 'indexes' into the GpuVertex*Data buffer
struct GpuIndexData {
  uint32_t index;
};

struct GpuVertexPositionData {
  glm::vec3 position;
};

struct GpuVertexColorData {
  glm::vec4 color;
};

struct GpuVertexTexCoordData {
  glm::vec2 texCoords;
};

struct GpuVertexNormalData {
  glm::vec3 normal;
};
}
