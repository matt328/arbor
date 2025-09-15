#pragma once

#include <cstdint>

namespace arb {

enum class QueueType : uint8_t {
  Compute = 0,
  Graphics,
  Transfer
};

}
