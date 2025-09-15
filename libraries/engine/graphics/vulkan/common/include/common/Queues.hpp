#pragma once

#include <cstdint>

namespace arb {

enum class Queues : uint8_t {
  Graphics = 0,
  Transfer,
  Compute,
  Count
};

}
