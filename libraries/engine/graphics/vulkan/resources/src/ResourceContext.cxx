#include "resources/ResourceContext.hpp"

#include "bk/Logger.hpp"
#include "BufferManager.hpp"

namespace arb {
ResourceContext::ResourceContext() {
  Log::trace("Constructing ResourceContext");
}

ResourceContext::~ResourceContext() {
  Log::trace("Destroying ResourceContext");
}
}
