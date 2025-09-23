#pragma once

#include "bk/NonCopyMove.hpp"
#include "common/SemaphorePack.hpp"

#include <memory>

namespace arb {

class BufferManager;
class AllocatorService;
class Semaphore;
class Device;
class ImageSystem;
class ResourceSystem;

class ResourceContext : public NonCopyableMovable {
public:
  ResourceContext(Device& newDevice, AllocatorService& allocatorService, SemaphorePack semaphores);
  ~ResourceContext();

  [[nodiscard]] auto getResourceFacade() const -> ResourceSystem&;

private:
  std::unique_ptr<BufferManager> bufferManager;
  std::unique_ptr<ImageSystem> imageSystem;
  std::unique_ptr<ResourceSystem> resourceSystem;
};

}
