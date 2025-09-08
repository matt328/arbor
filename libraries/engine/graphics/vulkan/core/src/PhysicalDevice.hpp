#pragma once

#include <array>

#include "DeviceFeatures.hpp"
#include "QueueFamilyIndices.hpp"

namespace arb {

class Device;
class Surface;

struct SwapchainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

static constexpr auto RequiredExtensions =
    std::to_array<const char*>({VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                                VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
                                VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
                                VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
                                VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
                                VK_EXT_EXTENDED_DYNAMIC_STATE_3_EXTENSION_NAME,
                                VK_EXT_DESCRIPTOR_BUFFER_EXTENSION_NAME,
                                VK_KHR_DEVICE_GROUP_EXTENSION_NAME,
                                VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
                                VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME});

class PhysicalDevice {
public:
  PhysicalDevice() = default;
  PhysicalDevice(VkPhysicalDevice handle);
  ~PhysicalDevice();

  PhysicalDevice(const PhysicalDevice&) = default;
  PhysicalDevice(PhysicalDevice&&) noexcept = default;
  auto operator=(const PhysicalDevice&) -> PhysicalDevice& = default;
  auto operator=(PhysicalDevice&&) noexcept -> PhysicalDevice& = default;

  static DeviceFeatures RequiredFeatures;

  [[nodiscard]] auto isSuitable(const Surface& surface) const -> bool;
  auto createDevice(const Surface& surface) -> std::shared_ptr<Device>;
  [[nodiscard]] auto handle() const -> VkPhysicalDevice;
  [[nodiscard]] auto findQueueFamilies(const Surface* surface) const -> QueueFamilyIndices;
  [[nodiscard]] auto querySwapchainSupport(const Surface* surface) const -> SwapchainSupportDetails;

private:
  VkPhysicalDevice vkPhysicalDevice{VK_NULL_HANDLE};
  VkPhysicalDeviceProperties properties{};

  auto getQueueCreateInfo(std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos,
                          const QueueFamilyIndices& queueFamilyIndices) -> void;
  [[nodiscard]] auto supportsExtensions() const -> bool;
  [[nodiscard]] auto supportsFeatures() const -> bool;

  template <typename FeatureStruct>
  void checkFeature(const char* name,
                    const FeatureStruct& required,
                    const FeatureStruct& supported,
                    auto MemberPtr) const {
    if (required.*MemberPtr && !(supported.*MemberPtr)) {
      throw std::runtime_error(std::string("Missing required feature: ") + name);
    }
  }

  static void logQueueFamilyIndices(const QueueFamilyIndices& qf);
};
}
