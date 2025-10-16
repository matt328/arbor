#pragma once

#include <chrono>

namespace Arbor {

using Clock = std::chrono::steady_clock;
using Timestamp = Clock::time_point;

}
