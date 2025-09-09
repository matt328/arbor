#pragma once

#include <cstdint>

namespace arb {

enum class ImageAcquireResult : uint8_t {
  Success,
  NeedsResize,
  Error
};

}
