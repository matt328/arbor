#pragma once

#include "buffers/BufferHandle.hpp"

namespace arb {

struct PerFrameBufferHandles {
  LogicalBufferHandle logicalHandle;
  std::vector<BufferHandle> bufferHandles;
};

}
