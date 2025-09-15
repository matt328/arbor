#include "BufferAllocator.hpp"

#include "ArenaAllocator.hpp"
#include "LinearAllocator.hpp"
#include "bk/Logger.hpp"

namespace arb {
BufferAllocator::BufferAllocator() {
  Log::trace("Constructing BufferAllocator");
}

BufferAllocator::~BufferAllocator() {
  Log::trace("Destroying BufferAllocator");
}

auto BufferAllocator::registerBuffer(BufferHandle bufferHandle,
                                     AllocationStrategy strategy,
                                     size_t initialSize,
                                     std::optional<std::string> debugName) -> void {
  switch (strategy) {
    case AllocationStrategy::Linear:
      allocatorMap.emplace(bufferHandle,
                           std::make_unique<LinearAllocator>(bufferHandle, initialSize, debugName));
      break;
    case AllocationStrategy::Arena:
      allocatorMap.emplace(bufferHandle,
                           std::make_unique<ArenaAllocator>(bufferHandle, initialSize, debugName));
      break;
    case AllocationStrategy::Resizable:

      break;
  }
}
}
