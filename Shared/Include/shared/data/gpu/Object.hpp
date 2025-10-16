#pragma once

#include <DirectXMath.h>

namespace Arbor::gpu {

using namespace DirectX;

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
  XMFLOAT3 position;
};

struct Rotation {
  XMVECTOR rotation;
};

struct alignas(4) Scale {
  XMFLOAT3 scale;
};

struct Animation {
  XMMATRIX jointMatrices;
};

struct Material {
  XMVECTOR baseColor;
  uint32_t albedoTextureId;
};

}
