#include "LogicalBufferMapper.hpp"

#include "bk/Log.hpp"

namespace arb {

LogicalBufferMapper::LogicalBufferMapper() {
  LOG_TRACE_L1(Log::Renderer, "Creating LogicalBufferManager");
}

LogicalBufferMapper::~LogicalBufferMapper() {
  LOG_TRACE_L1(Log::Renderer, "Destroying LogicalBufferManager");
}

void LogicalBufferMapper::registerPerFrameBuffer(LogicalBufferHandle logicalHandle,
                                                 BufferHandle handle,
                                                 uint32_t frameIndex) {
  mapping[logicalHandle][frameIndex] = handle;
}

auto LogicalBufferMapper::resolveBufferHandle(LogicalBufferHandle logicalHandle,
                                              uint32_t frameIndex) -> BufferHandle {
  return mapping.at(logicalHandle).at(frameIndex);
}

void LogicalBufferMapper::removeBuffer(LogicalBufferHandle logicalHandle) {
  mapping.erase(logicalHandle);
}

}
