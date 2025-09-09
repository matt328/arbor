#pragma once

#include <string_view>
#include <vector>
#include <vulkan/vulkan.h>

#include "graphics/common/GraphicsOptions.hpp"
#include "bk/NonCopyMove.hpp"

namespace arb {

class Surface;
class PhysicalDevice;

class Instance : public NonCopyable {
public:
  Instance(const GraphicsOptions& newOptions);
  ~Instance();

  Instance(Instance&& other) noexcept
      : vkInstance(other.vkInstance), debugMessenger(other.debugMessenger) {
    other.vkInstance = VK_NULL_HANDLE;
    other.debugMessenger = VK_NULL_HANDLE;
  }

  auto operator=(Instance&& other) noexcept -> Instance& {
    if (this != &other) {
      cleanup();
      vkInstance = other.vkInstance;
      debugMessenger = other.debugMessenger;
      other.vkInstance = VK_NULL_HANDLE;
      other.debugMessenger = VK_NULL_HANDLE;
    }
    return *this;
  }

  auto enumeratePhysicalDevices(const Surface& surface) -> std::vector<PhysicalDevice>;
  [[nodiscard]] auto getHandle() const -> VkInstance;
  operator VkInstance() const {
    return vkInstance;
  }

private:
  VkInstance vkInstance{VK_NULL_HANDLE};
  VkDebugUtilsMessengerEXT debugMessenger{VK_NULL_HANDLE};
  GraphicsOptions graphicsOptions;

  [[nodiscard]] auto getInstanceExtensions() const -> std::vector<const char*>;
  [[nodiscard]] static auto getAvailableLayers() -> std::vector<VkLayerProperties>;
  [[nodiscard]] static auto isLayerAvailable(std::string_view layerName) -> bool;

  static VKAPI_ATTR auto VKAPI_CALL
  debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                VkDebugUtilsMessageTypeFlagsEXT type,
                const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
                void* userData) -> VkBool32;

  void cleanup() {
    if (vkInstance != VK_NULL_HANDLE) {
      vkDestroyInstance(vkInstance, nullptr);
      vkInstance = VK_NULL_HANDLE;
    }
  }
};

}
