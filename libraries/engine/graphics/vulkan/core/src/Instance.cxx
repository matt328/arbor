#include "Instance.hpp"

#include <algorithm>

#include "bk/Logger.hpp"

#include "common/ErrorUtils.hpp"
#include "graphics/common/GraphicsOptions.hpp"
#include "PhysicalDevice.hpp"

namespace arb {

Instance::Instance(const GraphicsOptions& newOptions) {
  Log::trace("Creating Vulkan Instance");
  graphicsOptions = newOptions;
  const auto appInfo = VkApplicationInfo{.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                                         .pApplicationName = "Arbor Application",
                                         .applicationVersion = VK_MAKE_VERSION(0, 0, 1),
                                         .pEngineName = "Arbor",
                                         .engineVersion = VK_MAKE_VERSION(0, 0, 1),
                                         .apiVersion = VK_API_VERSION_1_3};

  auto instanceExtensions = getInstanceExtensions();
  auto createInfo = VkInstanceCreateInfo{.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                         .pApplicationInfo = &appInfo,
                                         .enabledExtensionCount =
                                             static_cast<uint32_t>(instanceExtensions.size()),
                                         .ppEnabledExtensionNames = instanceExtensions.data()};

  const auto* const validationLayerName = "VK_LAYER_KHRONOS_validation";

  if (graphicsOptions.debugEnabled) {
    if (!isLayerAvailable(validationLayerName)) {
      throw std::runtime_error("Debug requested but VK_LAYER_KHRONOS_validation is not available");
    }
    std::array layers = {validationLayerName};
    createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
    createInfo.ppEnabledLayerNames = layers.data();
  }

  checkVk(vkCreateInstance(&createInfo, nullptr, &vkInstance), "vkCreateInstance()");
  Log::trace("Created Vulkan Instance");

  if (graphicsOptions.debugEnabled) {
    auto debugCreateInfo = VkDebugUtilsMessengerCreateInfoEXT{
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debugCallback,
        .pUserData = nullptr,
    };

    auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(vkInstance, "vkCreateDebugUtilsMessengerEXT"));
    if (func != nullptr) {
      checkVk(func(vkInstance, &debugCreateInfo, nullptr, &debugMessenger),
              "vkCreateDebugUtilsMessengerEXT");
    } else {
      throw std::runtime_error(
          "vkGetInstanceProcAddr failed to acquire vkCreateDebugUtilsMessengerEXT");
    }
    Log::trace("Registered Debug Messenger");
  }
}

Instance::~Instance() {
  Log::trace("Destroying Vulkan Instance");
  if (vkInstance != nullptr) {
    if (graphicsOptions.debugEnabled && debugMessenger != nullptr) {
      auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
          vkGetInstanceProcAddr(vkInstance, "vkDestroyDebugUtilsMessengerEXT"));
      if (func != nullptr) {
        func(vkInstance, debugMessenger, nullptr);
      } else {
        Log::warn("vkGetInstanceProcAddr failed to acquire vkDestroyDebugUtilsMessengerEXT");
      }
    }
    vkDestroyInstance(vkInstance, nullptr);
  }
}

auto Instance::enumeratePhysicalDevices(const Surface& surface) -> std::vector<PhysicalDevice> {
  uint32_t deviceCount = 0;
  checkVk(vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr),
          "vkEnumeratePhysicalDevices(count)");

  if (deviceCount == 0) {
    throw std::runtime_error("No Vulkan physical devices found");
  }

  auto handles = std::vector<VkPhysicalDevice>{deviceCount};
  checkVk(vkEnumeratePhysicalDevices(vkInstance, &deviceCount, handles.data()),
          "vkEnumeratePhysicalDevices(devices)");

  auto physicalDevices = std::vector<PhysicalDevice>{};
  for (auto* const handle : handles) {
    physicalDevices.emplace_back(handle);
  }

  return physicalDevices;
}

auto Instance::getInstanceExtensions() const -> std::vector<const char*> {
  std::vector<const char*> extNames{VK_KHR_SURFACE_EXTENSION_NAME,
                                    VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
                                    VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
                                    VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME};

  if (graphicsOptions.debugEnabled) {
    extNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  uint32_t propertyCount = 0;
  checkVk(vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, nullptr),
          "vkEnumerateInstanceExtensionProperties(count)");

  if (propertyCount > 0) {
    std::vector<VkExtensionProperties> properties(propertyCount);
    checkVk(vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, properties.data()),
            "vkEnumerateInstanceExtensionProperties(fetch properties)");

    for (const auto* req : extNames) {
      bool found = std::ranges::any_of(properties, [req](const VkExtensionProperties& prop) {
        return std::string_view(prop.extensionName) == req;
      });

      if (!found) {
        throw std::runtime_error(std::format(
            "Instance::getInstanceExtensions(): Required Vulkan extension '{}' is not available",
            req));
      }
    }
  }

  return extNames;
}

auto Instance::getHandle() const -> VkInstance {
  return vkInstance;
}

VKAPI_ATTR auto VKAPI_CALL
Instance::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                        VkDebugUtilsMessageTypeFlagsEXT type,
                        const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
                        void* userData) -> VkBool32 {

  Log::debug("Vulkan Validation: {}", callbackData->pMessage);
  return VK_FALSE;
}

[[nodiscard]] auto Instance::getAvailableLayers() -> std::vector<VkLayerProperties> {
  uint32_t count = 0;
  checkVk(vkEnumerateInstanceLayerProperties(&count, nullptr),
          "vkEnumerateInstanceLayerProperties(count)");
  auto layers = std::vector<VkLayerProperties>{};
  if (count > 0) {
    layers.resize(count);
    checkVk(vkEnumerateInstanceLayerProperties(&count, layers.data()),
            "vkEnumerateInstanceLayerProperties(data)");
  }
  return layers;
}

[[nodiscard]] auto Instance::isLayerAvailable(std::string_view layerName) -> bool {
  auto layers = getAvailableLayers();
  return std::ranges::any_of(layers, [&](const VkLayerProperties& prop) {
    return layerName == prop.layerName;
  });
}

}
