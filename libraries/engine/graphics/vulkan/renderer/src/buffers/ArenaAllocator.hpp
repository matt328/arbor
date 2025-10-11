#pragma once

#include <string>

#include "buffers/BufferRegion.hpp"
#include "buffers/IBufferAllocator.hpp"
#include "buffers/BufferHandle.hpp"
#include "buffers/BufferRequest.hpp"

namespace arb {

class ArenaAllocator : public IBufferAllocator {
public:
  ArenaAllocator(BufferHandle bufferHandle,
                 size_t newInitialSize,
                 std::optional<std::string> newName = std::nullopt);
  ~ArenaAllocator() override = default;

  auto allocate(const BufferRequest& requestData) -> BufferRegion override;
  auto checkSize(const BufferRequest& requestData) -> std::optional<ResizeRequest> override;
  auto notifyBufferResized(size_t newSize) -> void override;
  auto freeRegion(const BufferRegion& region) -> void override;
  auto reset() -> void override;

private:
  size_t currentBufferSize{};
  size_t currentOffset = 0;

  std::optional<std::string> name;
};

}
