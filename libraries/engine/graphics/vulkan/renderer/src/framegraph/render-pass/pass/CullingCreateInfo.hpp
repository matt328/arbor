#pragma once

#include "framegraph/ResourceAliases.hpp"

namespace arb {
struct CullingCreateInfo {
  BufferAlias objectDataBuffer;
  BufferAlias geometryRegionBuffer;
  BufferAlias indirectCommandBuffer;
  BufferAlias indirectCommandCountBuffer;
};
}
