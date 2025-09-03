#include "Surface.hpp"
#include "ErrorUtils.hpp"
#include "Instance.hpp"

namespace arb {

Surface::Surface(std::shared_ptr<Instance> newInstance, void* hwnd)
    : instance{std::move(newInstance)} {
  const auto createInfo = VkWin32SurfaceCreateInfoKHR{
      .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
      .hinstance = GetModuleHandle(nullptr),
      .hwnd = static_cast<HWND>(hwnd),
  };
  checkVk(vkCreateWin32SurfaceKHR(*instance, &createInfo, nullptr, &vkSurface),
          "createWin32SurfaceKHR");
}

Surface::~Surface() {
  vkDestroySurfaceKHR(*instance, vkSurface, nullptr);
}

}
