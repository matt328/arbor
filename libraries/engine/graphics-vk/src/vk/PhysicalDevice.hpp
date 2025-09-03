#pragma once

#include "QueueFamilyIndices.hpp"

namespace arb {

class Device;
class Surface;

class PhysicalDevice {
public:
  PhysicalDevice() = default;
  PhysicalDevice(VkPhysicalDevice handle);
  ~PhysicalDevice();

  PhysicalDevice(const PhysicalDevice&) = default;
  PhysicalDevice(PhysicalDevice&&) noexcept = default;
  auto operator=(const PhysicalDevice&) -> PhysicalDevice& = default;
  auto operator=(PhysicalDevice&&) noexcept -> PhysicalDevice& = default;

  auto createDevice(const Surface& surface) -> std::shared_ptr<Device>;

  [[nodiscard]] auto handle() const -> VkPhysicalDevice;

private:
  VkPhysicalDevice vkPhysicalDevice{VK_NULL_HANDLE};
  VkPhysicalDeviceProperties properties{};

  auto findQueueFamilies(const Surface& surface) -> QueueFamilyIndices;
  auto getQueueCreateInfo(std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos,
                          const QueueFamilyIndices& queueFamilyIndices) -> void;

  static void logQueueFamilyIndices(const QueueFamilyIndices& qf) {
    auto logFamily = [](const char* name,
                        std::optional<uint32_t> index,
                        std::optional<uint32_t> count,
                        const std::vector<float>& priorities) {
      if (index.has_value()) {
        std::string prioStr;
        for (size_t i = 0; i < priorities.size(); ++i) {
          prioStr += std::to_string(priorities[i]);
          if (i + 1 < priorities.size())
            prioStr += ", ";
        }
        Log->trace("{}: index={} count={} priorities=[{}]", name, *index, *count, prioStr);
      } else {
        Log->trace("{}: not found", name);
      }
    };

    logFamily("Graphics", qf.graphicsFamily, qf.graphicsFamilyCount, qf.graphicsFamilyPriorities);
    logFamily("Compute", qf.computeFamily, qf.computeFamilyCount, qf.computeFamilyPriorities);
    logFamily("Transfer", qf.transferFamily, qf.transferFamilyCount, qf.transferFamilyPriorities);
    logFamily("Present", qf.presentFamily, qf.presentFamilyCount, qf.presentFamilyPriorities);
  }
};
}
