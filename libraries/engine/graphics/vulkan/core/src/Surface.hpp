#pragma once

#include <vulkan/vulkan.h>

#include "bk/NonCopyMove.hpp"

namespace arb {

class Instance;

class Surface : NonCopyableMovable {
public:
  Surface(VkInstance newInstance, void* hwnd);
  ~Surface();

  operator VkSurfaceKHR() const {
    return vkSurface;
  }

private:
  VkSurfaceKHR vkSurface{};
  VkInstance instance;
};

}
