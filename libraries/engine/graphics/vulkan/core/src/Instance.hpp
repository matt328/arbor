#pragma once

#include "graphics/common/GraphicsOptions.hpp"

namespace arb {

class Surface;
class PhysicalDevice;

class Instance : public NonCopyableMovable {
public:
  Instance(const GraphicsOptions& newOptions);
  ~Instance();

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
};

}
