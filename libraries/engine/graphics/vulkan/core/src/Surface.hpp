#pragma once

namespace arb {

class Instance;

class Surface : NonCopyableMovable {
public:
  Surface(std::shared_ptr<Instance> newInstance, void* hwnd);
  ~Surface();

  operator VkSurfaceKHR() const {
    return vkSurface;
  }

private:
  VkSurfaceKHR vkSurface{};
  std::shared_ptr<Instance> instance;
};

}
