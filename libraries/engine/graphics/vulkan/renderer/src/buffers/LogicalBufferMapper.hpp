#pragma once

#include "buffers/BufferHandle.hpp"

namespace arb {

class LogicalBufferMapper {
public:
  LogicalBufferMapper();
  ~LogicalBufferMapper();

  void registerPerFrameBuffer(LogicalBufferHandle logicalHandle,
                              BufferHandle handle,
                              uint32_t frameIndex);
  auto resolveBufferHandle(LogicalBufferHandle logicalHandle, uint32_t frameIndex) -> BufferHandle;
  void removeBuffer(LogicalBufferHandle logicalHandle);

private:
  std::unordered_map<LogicalBufferHandle, std::unordered_map<uint32_t, BufferHandle>> mapping;
};

}
