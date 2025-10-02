#pragma once

#include <vector>
#include "BufferRequest.hpp"
#include "common/ImageRequirement.hpp"

namespace arb {

class IDispatcher;

struct PassDescription {
  std::string name;
  std::vector<ImageRequirement> images;
  std::vector<BufferRequest2> buffers;
  std::vector<std::string> dependsOn;
};

}
