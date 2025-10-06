#pragma once

#include <memory>
#include <optional>
#include <unordered_map>

#include "bk/NonCopyMove.hpp"
#include "common/BufferCreateInfo.hpp"
#include "resources/BufferHandle.hpp"

namespace arb {

class IBufferAllocator;

class BufferAllocator : public NonCopyableMovable {
public:
  BufferAllocator();
  ~BufferAllocator();

  auto registerBuffer(BufferHandle bufferHandle,
                      AllocationStrategy strategy,
                      size_t initialSize,
                      std::optional<std::string> debugName = std::nullopt) -> void;

private:
  std::unordered_map<BufferHandle, std::unique_ptr<IBufferAllocator>> allocatorMap;
};

}
