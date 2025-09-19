#pragma once

#include <vector>
#include "framegraph/ImageAlias.hpp"

namespace arb {

struct ForwardCreateInfo {
  ImageAlias colorImage;
  ImageAlias depthImage;
  // std::vector<Handle<DSLayout>> dsLayoutHandles;
};

}
