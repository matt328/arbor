#include "assets/AssetContext.hpp"

#include "bk/Log.hpp"

namespace arb {

AssetContext::AssetContext() {
  LOG_TRACE_L1(Log::Assets, "Constructing AssetContext");
}

AssetContext::~AssetContext() {
  LOG_TRACE_L1(Log::Assets, "Destroying AssetContext");
}

}
