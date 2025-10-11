#pragma once

#include "bk/NonCopyMove.hpp"
#include "core/Buffer.hpp"
#include "buffers/BufferHandle.hpp"

namespace arb {

class BufferSystem;
class BufferRequest;

struct GlobalBuffers {
  LogicalBufferHandle frameData;
  LogicalBufferHandle resourceTable;
  LogicalBufferHandle objectData;
  LogicalBufferHandle objectPositions;
  LogicalBufferHandle objectRotations;
  LogicalBufferHandle objectScales;
  LogicalBufferHandle geometryRegion;
  LogicalBufferHandle materials;
  LogicalBufferHandle drawCommands;
  LogicalBufferHandle drawCounts;
};

class GlobalBufferManager : public NonCopyableMovable {
public:
  explicit GlobalBufferManager(BufferSystem& newBufferSystem);
  ~GlobalBufferManager();

  [[nodiscard]] auto getGlobalBuffers() const -> const GlobalBuffers&;

  [[nodiscard]] auto resolve(LogicalBufferHandle handle, uint32_t frameIndex) -> Buffer&;

private:
  BufferSystem& bufferSystem;
  GlobalBuffers globalBuffers;

  void createGlobalBuffers();
  void destroyGlobalBuffers();
};

}
