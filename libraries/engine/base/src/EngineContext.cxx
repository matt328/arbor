#include "EngineContext.hpp"
#include "gameplay/IGameplayContext.hpp"
#include "graphics/base/IGraphicsContext.hpp"
#include "graphics/common/GraphicsOptions.hpp"
#include "bk/EventQueue2.hpp"
#include "engine/common/EngineOptions.hpp"

namespace arb {
EngineContext::EngineContext(bk::NativeWindowHandle newWindowHandle, EngineOptions engineOptions)
    : windowHandle{newWindowHandle} {
  Log->trace("Creating EngineContext");

  eventQueue = std::make_shared<bk::EventQueue>();

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

  graphicsThread = std::jthread([this, &engineOptions](std::stop_token token) {
    try {
      InitLogger("Graphics");
      Log->trace("Graphics Thread Started");
      EngineOptions::Size size = engineOptions.initialSize;
      auto graphicsContext = makeGraphicsContext(
          eventQueue,
          GraphicsOptions{.debugEnabled = engineOptions.debugEnabled,
                          .initialSize = {.width = engineOptions.initialSize.width,
                                          .height = engineOptions.initialSize.height}},
          windowHandle);
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

auto EngineContext::getEventQueue() const -> std::shared_ptr<bk::IEventQueue> {
  return eventQueue;
}

auto makeEngineContext(bk::NativeWindowHandle newWindowHandle, EngineOptions engineOptions)
    -> std::shared_ptr<IEngineContext> {
  return std::make_shared<EngineContext>(newWindowHandle, engineOptions);
}

}
