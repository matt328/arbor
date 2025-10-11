#include "GlobalBufferManager.hpp"

#include "bk/Log.hpp"
#include "buffers/BufferSytem.hpp"

namespace arb {

GlobalBufferManager::GlobalBufferManager(BufferSystem& newBufferSystem)
    : bufferSystem{newBufferSystem}, globalBuffers{} {
  LOG_TRACE_L1(Log::Renderer, "Creating GlobalBufferManager");
  createGlobalBuffers();
}

GlobalBufferManager::~GlobalBufferManager() {
  LOG_TRACE_L1(Log::Renderer, "Destroying GlobalBufferManager");
}

auto GlobalBufferManager::getGlobalBuffers() const -> const GlobalBuffers& {
  return globalBuffers;
}

auto GlobalBufferManager::resolve(LogicalBufferHandle handle, uint32_t frameIndex) const
    -> Buffer& {
  return bufferSystem.getBuffer(handle, frameIndex);
}

void GlobalBufferManager::createGlobalBuffers() {
  auto buffers = bufferSystem.registerPerFrameBuffer(
      BufferCreateInfo{.bufferLifetime = BufferLifetime::Transient,
                       .initialSize = 20480,
                       .debugName = "DrawCommands",
                       .indirect = true},
      3);
}

}
