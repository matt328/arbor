#pragma once

#include <vector>
#include "common/BufferRequirement.hpp"
#include "common/ImageRequirement.hpp"

namespace arb {

class IDispatcher;

struct PassDescription {
  std::string name;
  std::vector<ImageRequirement> images;
  std::vector<BufferRequirement> buffers;
  std::vector<std::string> dependsOn;
};

}
