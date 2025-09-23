#pragma once

#include "resources/BufferHandle.hpp"
#include "framegraph/ComponentIds.hpp"

#include "ForwardDispatcher.hpp"

namespace arb {

class ResourceSystem;

struct ForwardCreateInfo {
  DispatcherId id;
  LogicalBufferHandle resourceTable;
  LogicalBufferHandle frameData;
  LogicalBufferHandle indirectCommand;
  LogicalBufferHandle indirectCommandCount;

  auto create(ResourceSystem& resourceSystem) const -> std::unique_ptr<ForwardDispatcher> {
    return nullptr;
  }
};

}
