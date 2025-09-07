#pragma once

namespace arb {

enum class ImageAcquireResult : uint8_t {
  Success,
  NeedsResize,
  Error
};

}
