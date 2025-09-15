#include "resources/ResourceContext.hpp"

#include "bk/Logger.hpp"
#include "buffers/BufferManager.hpp"
#include "resources/ResourceFacade.hpp"

namespace arb {

ResourceContext::ResourceContext(Device& newDevice,
                                 AllocatorService& allocatorService,
                                 SemaphorePack semaphores) {
  Log::trace("Constructing ResourceContext");
  bufferManager = std::make_unique<BufferManager>(newDevice, allocatorService, semaphores);
  resourceFacade = std::make_unique<ResourceFacade>(*bufferManager);
}

ResourceContext::~ResourceContext() {
  Log::trace("Destroying ResourceContext");
}

auto ResourceContext::getResourceFacade() const -> ResourceFacade& {
  return *resourceFacade;
}

}
