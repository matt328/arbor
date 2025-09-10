#include <chrono>
#include <thread>

#include "engine/base/IEngineContext.hpp"
#include "bk/Logger.hpp"

auto main() -> int {
  InitLogger("Main");
  Log::info("Logger Initialized");

  uint32_t dummyWindowId = 5;
  auto windowId = bk::NativeWindowHandle{};
  windowId.set(&dummyWindowId);

  auto context = arb::makeEngineContext(windowId, {});

  auto eventQueue = context->getEventQueue();

  using namespace std::chrono;
  auto start = steady_clock::now();
  auto end = start + seconds(3);

  try {
    while (steady_clock::now() < end) {
      context->update();
      // sleep a short time to avoid pegging CPU
      std::this_thread::sleep_for(nanoseconds(1000)); // 1µs
    }
  } catch (const std::exception& e) {
    Log::error("Exception in engine context: {}", e.what());
    return -1;
  }

  return 0;
}
