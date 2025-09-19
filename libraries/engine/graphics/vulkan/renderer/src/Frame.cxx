#include "Frame.hpp"

#include "bk/Logger.hpp"
#include "FrameManager.hpp"

namespace arb {

Frame::Frame(FrameManager* newFrameManager, uint8_t newIndex)
    : frameManager{newFrameManager}, index{newIndex} {
  Log::trace("Creating Frame index={}", newIndex);
}

Frame::~Frame() {
  Log::trace("Destroying Frame index={}", index);
}

}
