#pragma once

#include "bk/NonCopyMove.hpp"

namespace arb {

class AssetContext : NonCopyableMovable {
public:
  AssetContext();
  ~AssetContext();
};

}
