#include "BufferAllocator.hpp"

#include "ArenaAllocator.hpp"
#include "LinearAllocator.hpp"

#include "bk/Log.hpp"

namespace arb {
BufferAllocator::BufferAllocator() {
  LOG_TRACE_L1(Log::Resources, "Constructing BufferAllocator");
}

BufferAllocator::~BufferAllocator() {
  LOG_TRACE_L1(Log::Resources, "Destroying BufferAllocator");
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
