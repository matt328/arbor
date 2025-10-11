#include "GlobalBufferManager.hpp"

#include "bk/Log.hpp"
#include "buffers/BufferSystem.hpp"

namespace arb {

GlobalBufferManager::GlobalBufferManager(BufferSystem& newBufferSystem)
    : bufferSystem{newBufferSystem}, globalBuffers{} {
  LOG_TRACE_L1(Log::Renderer, "Creating GlobalBufferManager");
  createGlobalBuffers();
}

GlobalBufferManager::~GlobalBufferManager() {
  LOG_TRACE_L1(Log::Renderer, "Destroying GlobalBufferManager");
  destroyGlobalBuffers();
}

auto GlobalBufferManager::getGlobalBuffers() const -> const GlobalBuffers& {
  return globalBuffers;
}

auto GlobalBufferManager::resolve(LogicalBufferHandle handle, uint32_t frameIndex) -> Buffer& {
  return bufferSystem.getPerFrameBuffer(handle, frameIndex);
}

void GlobalBufferManager::createGlobalBuffers() {
  globalBuffers.drawCommands = bufferSystem.registerPerFrameBuffer(
      BufferCreateInfo{.bufferLifetime = BufferLifetime::Transient,
                       .initialSize = 20480,
                       .debugName = "DrawCommands",
                       .indirect = true},
      3);
  globalBuffers.drawCounts = bufferSystem.registerPerFrameBuffer(
      BufferCreateInfo{.bufferLifetime = BufferLifetime::Transient,
                       .initialSize = 4,
                       .debugName = "DrawCounts",
                       .indirect = true},
      3);

  globalBuffers.objectData = bufferSystem.registerPerFrameBuffer(
      BufferCreateInfo{.bufferLifetime = BufferLifetime::Transient,
                       .initialSize = 40960,
                       .debugName = "Buffer-ObjectData"},
      3);

  globalBuffers.objectPositions = bufferSystem.registerPerFrameBuffer(
      BufferCreateInfo{.bufferLifetime = BufferLifetime::Transient,
                       .initialSize = 40960,
                       .debugName = "Buffer-ObjectPositions"},
      3);

  globalBuffers.objectRotations = bufferSystem.registerPerFrameBuffer(
      BufferCreateInfo{.bufferLifetime = BufferLifetime::Transient,
                       .initialSize = 40960,
                       .debugName = "Buffer-ObjectRotations"},
      3);

  globalBuffers.objectScales = bufferSystem.registerPerFrameBuffer(
      BufferCreateInfo{.bufferLifetime = BufferLifetime::Transient,
                       .initialSize = 40960,
                       .debugName = "Buffer-ObjectScales"},
      3);

  globalBuffers.geometryRegion = bufferSystem.registerPerFrameBuffer(
      BufferCreateInfo{.bufferLifetime = BufferLifetime::Transient,
                       .initialSize = 8192,
                       .debugName = "Buffer-GeometryRegion"},
      3);

  globalBuffers.materials =
      bufferSystem.registerPerFrameBuffer({.bufferLifetime = BufferLifetime::Transient,
                                           .initialSize = 40960,
                                           .debugName = "Buffer-Materials"},
                                          3);

  globalBuffers.frameData = bufferSystem.registerPerFrameBuffer(
      BufferCreateInfo{
          .bufferLifetime = BufferLifetime::Transient,
          .bufferUsage = BufferUsage::Storage,
          .initialSize = 40960,
          .debugName = "Buffer-FrameData",
      },
      3);

  globalBuffers.resourceTable = bufferSystem.registerPerFrameBuffer(
      BufferCreateInfo{
          .bufferLifetime = BufferLifetime::Transient,
          .bufferUsage = BufferUsage::Storage,
          .debugName = "Buffer-ResourceTable",
      },
      3);
}

void GlobalBufferManager::destroyGlobalBuffers() {
}

}
