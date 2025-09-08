#pragma once

#include <chrono>

namespace arb {

using Clock = std::chrono::steady_clock;
using Timestamp = Clock::time_point;

}
