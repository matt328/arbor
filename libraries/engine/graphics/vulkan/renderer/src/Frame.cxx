#include "Frame.hpp"

#include "bk/Log.hpp"
#include "FrameManager.hpp"

namespace arb {

Frame::Frame(FrameManager* newFrameManager, uint8_t newIndex)
    : frameManager{newFrameManager}, index{newIndex} {
  LOG_TRACE_L1(Log::Renderer, "Creating Frame index={}", newIndex);
}

Frame::~Frame() {
  LOG_TRACE_L1(Log::Renderer, "Destroying Frame index={}", index);
}

}
