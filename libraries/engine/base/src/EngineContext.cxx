#include "EngineContext.hpp"
#include "gameplay/IGameplayContext.hpp"
#include "graphics/base/IGraphicsContext.hpp"
#include "graphics/common/GraphicsOptions.hpp"
#include "bk/EventQueue2.hpp"
#include "engine/common/EngineOptions.hpp"
#include "engine/common/TripleBuffer.hpp"

namespace arb {

EngineContext::EngineContext(bk::NativeWindowHandle newWindowHandle, EngineOptions engineOptions)
    : windowHandle{newWindowHandle} {
  Log::trace("Creating EngineContext");

  eventQueue = std::make_shared<bk::EventQueue>();
  simStateBuffer = std::make_unique<TripleBuffer<SimState>>();

  gameThread = std::jthread([this](std::stop_token token) {
    try {
      InitLogger("Game");
      Log::trace("Game Thread Started");
      auto gameplayContext = makeGameplayContext(eventQueue, *simStateBuffer);
      gameplayContext->run(token);
    } catch (const std::exception& e) {
      Log::trace("Gameplay Thread Exception: {}", e.what());
      engineError = std::current_exception();
      requestStop();
    }
  });

  graphicsThread = std::jthread([this, engineOptions](std::stop_token token) {
    try {
      InitLogger("Graphics");
      Log::trace("Graphics Thread Started");
      EngineOptions::Size size = engineOptions.initialSize;
      auto graphicsContext = makeGraphicsContext(
          eventQueue,
          *simStateBuffer,
          GraphicsOptions{.debugEnabled = engineOptions.debugEnabled,
                          .initialSize = {.width = engineOptions.initialSize.width,
                                          .height = engineOptions.initialSize.height},
                          .framesInFlight = 3},

          windowHandle);
      graphicsContext->run(token);
    } catch (const std::exception& e) {
      Log::trace("Graphics Thread Exception: {}", e.what());
      engineError = std::current_exception();
      requestStop();
    }
  });
}

EngineContext::~EngineContext() {
  Log::trace("Destroying EngineContext");
}

auto EngineContext::update() -> void {
  if (engineError) {
    std::rethrow_exception(engineError);
  }
}

auto EngineContext::requestStop() -> void {
  Log::trace("Stop Requested");
  if (gameThread.joinable()) {
    Log::trace("GameThread joinable");
    gameThread.request_stop();
  }
  if (graphicsThread.joinable()) {
    Log::trace("GraphicsThread joinable");
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
