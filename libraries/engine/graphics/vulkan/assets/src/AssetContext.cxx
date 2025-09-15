#include "assets/AssetContext.hpp"

#include "bk/Logger.hpp"

namespace arb {

AssetContext::AssetContext() {
  Log::trace("Constructing AssetContext");
}

AssetContext::~AssetContext() {
  Log::trace("Destroying AssetContext");
}

}
