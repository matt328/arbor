#include "resources/ResourceContext.hpp"

#include "bk/Logger.hpp"
#include "buffers/BufferManager.hpp"
#include "resources/ResourceSystem.hpp"

namespace arb {

ResourceContext::ResourceContext(Device& newDevice,
                                 AllocatorService& allocatorService,
                                 SemaphorePack semaphores) {
  Log::trace("Constructing ResourceContext");
  bufferManager = std::make_unique<BufferManager>(newDevice, allocatorService, semaphores);
  imageSystem = std::make_unique<ImageSystem>(newDevice, allocatorService);
  resourceSystem = std::make_unique<ResourceSystem>(*bufferManager, *imageSystem);
}

ResourceContext::~ResourceContext() {
  Log::trace("Destroying ResourceContext");
}

auto ResourceContext::getResourceFacade() const -> ResourceSystem& {
  return *resourceSystem;
}

}
