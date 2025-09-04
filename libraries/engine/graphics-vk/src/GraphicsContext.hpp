#pragma once

#include "IGraphicsContext.hpp"

namespace arb {

class GraphicsContext : public IGraphicsContext {
public:
  GraphicsContext(std::shared_ptr<IEventQueue> newEventQueue,
                  const GraphicsOptions& newOptions,
                  bk::NativeWindowHandle newWindowHandle);
  ~GraphicsContext() override;

  auto run(std::stop_token token) -> void override;
};

}
