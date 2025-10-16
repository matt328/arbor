#pragma once

namespace Arbor {

struct RenderAreaCreated {
  uint32_t width;
  uint32_t height;
};

struct RenderAreaResized {
  uint32_t width;
  uint32_t height;
};

}
