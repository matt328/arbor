#pragma once

#include "resources/BufferHandle.hpp"
#include "framegraph/ComponentIds.hpp"
#include "CullingDispatcher.hpp"

namespace arb {

class ResourceFacade;

struct CullingCreateInfo {
  DispatcherId id;
  LogicalBufferHandle resourceTable;
  LogicalBufferHandle frameData;

  auto create(ResourceFacade& resourceFacade) const -> std::unique_ptr<CullingDispatcher> {
    return nullptr;
  }
};

}
