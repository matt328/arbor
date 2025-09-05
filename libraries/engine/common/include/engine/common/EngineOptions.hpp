#pragma once

namespace arb {

struct EngineOptions {
  struct Size {
    uint32_t width;
    uint32_t height;
  };
  bool debugEnabled{false};
  Size initialSize{.width = 800, .height = 600};
};

}
