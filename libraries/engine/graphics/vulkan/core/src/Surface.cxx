#include "Surface.hpp"

#include "bk/Logger.hpp"

#include "ErrorUtils.hpp"
#include "Instance.hpp"

namespace arb {

Surface::Surface(VkInstance newInstance, void* hwnd) : instance{newInstance} {
  const auto createInfo = VkWin32SurfaceCreateInfoKHR{
      .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
      .hinstance = GetModuleHandle(nullptr),
      .hwnd = static_cast<HWND>(hwnd),
  };
  Log::trace("Creating Surface");
  checkVk(vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &vkSurface),
          "createWin32SurfaceKHR");
}

Surface::~Surface() {
  Log::trace("Destroying Surface");
  vkDestroySurfaceKHR(instance, vkSurface, nullptr);
}

}
