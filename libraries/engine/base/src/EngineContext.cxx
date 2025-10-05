#include "EngineContext.hpp"

#include <cpptrace/cpptrace.hpp>
#include <cpptrace/from_current.hpp>

#include "gameplay/IGameplayContext.hpp"
#include "graphics/base/IGraphicsContext.hpp"
#include "engine/common/EngineOptions.hpp"
#include "bk/EventQueue2.hpp"
#include "engine/common/EngineOptions.hpp"
#include "engine/common/TripleBuffer.hpp"

namespace arb {

EngineContext::EngineContext(bk::NativeWindowHandle newWindowHandle, EngineOptions engineOptions)
    : windowHandle{newWindowHandle} {
  LOG_TRACE_L1(Log::Core, "Creating EngineContext");

  eventQueue = std::make_shared<bk::EventQueue>();
  simStateBuffer = std::make_unique<TripleBuffer<SimState>>();

  gameThread = std::jthread([this](std::stop_token token) {
    SetThreadDescription(GetCurrentThread(), L"Gameplay");
    try {
      LOG_TRACE_L1(Log::Core, "Game Thread Started");
      auto gameplayContext = makeGameplayContext(eventQueue, *simStateBuffer);
      gameplayContext->run(token);
    } catch (const std::exception& e) {
      LOG_TRACE_L1(Log::Core, "Gameplay Thread Exception: {}", e.what());
      engineError = std::current_exception();
      requestStop();
    }
  });

  graphicsThread = std::jthread([this, engineOptions](std::stop_token token) {
    SetThreadDescription(GetCurrentThread(), L"Graphics");
    try {
      LOG_TRACE_L1(Log::Core, "Graphics Thread Started");
      auto graphicsContext = makeGraphicsContext(
          eventQueue,
          *simStateBuffer,
          EngineOptions{.debugEnabled = engineOptions.debugEnabled,
                        .initialSurfaceState = engineOptions.initialSurfaceState},

          windowHandle);
      graphicsContext->run(token);
    } catch (const std::exception& e) {
      engineError = std::current_exception();
      requestStop();
    }
  });
}

EngineContext::~EngineContext() {
  LOG_TRACE_L1(Log::Core, "Destroying EngineContext");
}

auto EngineContext::update() -> void {
  if (engineError) {
    std::rethrow_exception(engineError);
  }
}

auto EngineContext::requestStop() -> void {
  if (gameThread.joinable()) {
    gameThread.request_stop();
  }
  if (graphicsThread.joinable()) {
    graphicsThread.request_stop();
  }
}

auto EngineContext::getEventQueue() const -> std::shared_ptr<bk::IEventQueue> {
  return eventQueue;
}

auto makeEngineContext(bk::NativeWindowHandle newWindowHandle, EngineOptions engineOptions)
    -> std::unique_ptr<IEngineContext> {
  return std::make_unique<EngineContext>(newWindowHandle, engineOptions);
}

}
