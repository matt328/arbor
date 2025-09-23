#pragma once

#include "resources/BufferHandle.hpp"
#include "framegraph/ComponentIds.hpp"
#include "CullingDispatcher.hpp"

namespace arb {

class ResourceSystem;

struct CullingCreateInfo {
  DispatcherId id;
  LogicalBufferHandle resourceTable;
  LogicalBufferHandle frameData;

  auto create(ResourceSystem& resourceSystem) const -> std::unique_ptr<CullingDispatcher> {
    return nullptr;
  }
};

}
