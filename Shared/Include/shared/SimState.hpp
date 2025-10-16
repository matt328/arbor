#pragma once

#include <DirectXMath.h>

#include "ChronoDefs.hpp"
#include "data/gpu/Object.hpp"
#include "Handle.hpp"
#include "data/Tags.hpp"

namespace Arbor {

struct ObjectHandles {
  Handle<Geometry> geometryHandle;
  std::optional<Handle<TextureTag>> textureHandle;
};

struct SimState {
  // Movable
  SimState(SimState&&) noexcept = default;
  auto operator=(SimState&&) noexcept -> SimState& = default;
  // Not copyable
  SimState(const SimState&) = delete;
  auto operator=(const SimState&) -> SimState& = delete;

  Timestamp timeStamp{};                   // 8
  uint64_t tag{};                          // 8
  std::vector<gpu::Object> objectMetadata; // 28
  std::vector<gpu::Transform> positions;   // 20
  std::vector<gpu::Rotation> rotations;    // 20
  std::vector<gpu::Scale> scales;          // 20

  /// Parallel vector to gpu::Object. Each element in this vector contains a Geometry and Texture
  /// handle for each gpu::Object in objectMetadata
  std::vector<ObjectHandles> stateHandles; // 8

  DirectX::XMMATRIX view;
  DirectX::XMMATRIX projection;

  // Pre-allocate memory for the vectors when creating a SimState
  explicit SimState(size_t initialCapacity = 0) {
    objectMetadata.reserve(initialCapacity);
    positions.reserve(initialCapacity);
    rotations.reserve(initialCapacity);
    scales.reserve(initialCapacity);
    stateHandles.reserve(initialCapacity);
  }

  auto ensureCapacity(size_t needed) {
    if (needed > objectMetadata.capacity()) {
      objectMetadata.reserve(needed);
      positions.reserve(needed);
      rotations.reserve(needed);
      scales.reserve(needed);
      stateHandles.reserve(needed);
    }
  }

  auto clear() {
    objectMetadata.clear();
    positions.clear();
    rotations.clear();
    scales.clear();
    stateHandles.clear();
  }
};

}
