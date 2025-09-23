#pragma once

#include <string>

namespace arb {

struct CullingCreateInfo {
  std::string objectDataBuffer;
  std::string geometryRegionBuffer;
  std::string indirectCommandBuffer;
  std::string indirectCommandCountBuffer;
};

}
