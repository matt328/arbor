#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace arb::gpu {

/// ObjectData Buffer
struct Object {
  uint32_t transformIndex;
  uint32_t rotationIndex;
  uint32_t scaleIndex;

  uint32_t geometryRegionId;
  uint32_t materialId;
  uint32_t animationId;
};

struct alignas(4) Transform {
  glm::vec3 position;
};

struct Rotation {
  glm::quat rotation;
};

struct alignas(4) Scale {
  glm::vec3 scale;
};

struct Animation {
  glm::mat4 jointMatrices;
};

struct Material {
  glm::vec4 baseColor;
  uint32_t albedoTextureId;
};

}
