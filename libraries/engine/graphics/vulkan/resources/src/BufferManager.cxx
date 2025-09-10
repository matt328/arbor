#include "BufferManager.hpp"

#include "bk/Logger.hpp"
#include "core/AllocatorService.hpp"

namespace arb {

BufferManager::BufferManager(AllocatorService& newAllocatorService)
    : allocatorService{newAllocatorService} {
  Log::trace("Creating BufferManager");
}

BufferManager::~BufferManager() {
  Log::trace("Destroying BufferManager");
}

}
