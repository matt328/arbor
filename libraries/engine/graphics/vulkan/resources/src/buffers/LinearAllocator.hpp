#pragma once

#include <string>

#include "IBufferAllocator.hpp"

namespace arb {

class LinearAllocator : public IBufferAllocator {
public:
  LinearAllocator(BufferHandle bufferHandle,
                  size_t bufferSize,
                  std::optional<std::string> newName = std::nullopt);
  ~LinearAllocator() override = default;

  auto allocate(const BufferRequest& bufferRequest) -> BufferRegion override;
  auto checkSize(const BufferRequest& requestData) -> std::optional<ResizeRequest> override;
  auto notifyBufferResized(size_t newSize) -> void override;
  auto freeRegion(const BufferRegion& region) -> void override;
  auto reset() -> void override;

private:
  size_t currentOffset{};
  size_t maxBufferSize{};
  std::optional<std::string> name;
};
}
