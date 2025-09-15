#pragma once

#include "resources/BufferHandle.hpp"

namespace arb {

struct PerFrameBufferHandles {
  LogicalBufferHandle logicalHandle;
  std::vector<BufferHandle> bufferHandles;
};

}
