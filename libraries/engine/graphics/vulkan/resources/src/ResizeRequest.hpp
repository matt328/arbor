#pragma once

#include "common/Handles.hpp"
#include "engine/common/Handle.hpp"

namespace arb {

struct ResizeRequest {
  Handle<BufferTag> bufferHandle;
  size_t newSize;
};

}
