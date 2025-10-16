#include "pch.h"

#include "EntryPoint.hpp"

#include "EngineContext.hpp"

namespace Arbor {

static std::unique_ptr<EngineContext> gEngineContext;

void initialize() {
  if (gEngineContext == nullptr) {
    OutputDebugStringA("Initializing Engine\n");
    gEngineContext = std::make_unique<EngineContext>(EngineOptions{});
  }
}

void shutdown() {
  if (gEngineContext != nullptr) {
    OutputDebugStringA("Shutting down Engine\n");
    gEngineContext.reset();
    gEngineContext = nullptr;
  }
}

}
