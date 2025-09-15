#pragma once

#include "Buffer.hpp"
#include "BufferHandle.hpp"
#include "bk/NonCopyMove.hpp"
#include "BufferCreateInfo.hpp"
#include "common/SemaphorePack.hpp"

#include <memory>

namespace arb {

class BufferManager;
class AllocatorService;
class Semaphore;
class Device;
class ResourceFacade;

class ResourceContext : public NonCopyableMovable {
public:
  ResourceContext(Device& newDevice, AllocatorService& allocatorService, SemaphorePack semaphores);
  ~ResourceContext();

  [[nodiscard]] auto getResourceFacade() const -> ResourceFacade&;

private:
  std::unique_ptr<BufferManager> bufferManager;
  std::unique_ptr<ResourceFacade> resourceFacade;
};

}
