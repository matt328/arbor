#include "EngineContext.hpp"
#include "IGameplayContext.hpp"
#include "IGraphicsContext.hpp"
#include "EventQueue2.hpp"
#include "GraphicsOptions.hpp"

namespace arb {
EngineContext::EngineContext(bk::NativeWindowHandle newWindowHandle)
    : windowHandle{newWindowHandle} {
  Log->trace("Creating EngineContext");

  eventQueue = std::make_shared<EventQueue>();

  gameThread = std::jthread([this](std::stop_token token) {
    try {
      InitLogger("Game");
      Log->trace("Game Thread Started");
      auto gameplayContext = makeGameplayContext(eventQueue);
      gameplayContext->run(token);
    } catch (const std::exception& e) {
      engineError = std::current_exception();
      requestStop();
    }
  });

  graphicsThread = std::jthread([this](std::stop_token token) {
    try {
      InitLogger("Graphics");
      Log->trace("Graphics Thread Started");
      auto graphicsContext =
          makeGraphicsContext(eventQueue, GraphicsOptions{.debugEnabled = true}, windowHandle);
      graphicsContext->run(token);
    } catch (const std::exception& e) {
      engineError = std::current_exception();
      requestStop();
    }
  });
}

EngineContext::~EngineContext() {
  Log->trace("Destroying EngineContext");
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

auto EngineContext::getEventQueue() const -> std::shared_ptr<IEventQueue> {
  return eventQueue;
}

auto makeEngineContext(bk::NativeWindowHandle newWindowHandle) -> std::shared_ptr<IEngineContext> {
  return std::make_shared<EngineContext>(newWindowHandle);
}

}
