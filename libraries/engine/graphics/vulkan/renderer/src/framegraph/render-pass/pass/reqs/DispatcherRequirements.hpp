#pragma once

#include <vector>

#include "BufferRequest.hpp"
#include "ImageRequest.hpp"

namespace arb {

struct DispatcherRequirements {
  std::vector<BufferRequest2> buffers;
  std::vector<ImageRequest> sampledImages;
  std::vector<ImageRequest> storageImages;
};

}
