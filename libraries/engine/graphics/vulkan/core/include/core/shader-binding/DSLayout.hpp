#pragma once

#include <string_view>
#include <vulkan/vulkan_core.h>

#include "bk/NonCopyMove.hpp"
#include "core/Device.hpp"

namespace arb {

class DSLayout : public NonCopyableMovable {
public:
  DSLayout(Device& newDevice,
           const VkDescriptorSetLayoutCreateInfo& info,
           std::string_view name = "Unnamed Layout");
  ~DSLayout() noexcept;

  explicit operator VkDescriptorSetLayout() const {
    return vkLayout;
  }

  [[nodiscard]] auto getLayoutSize() const -> VkDeviceSize;
  [[nodiscard]] auto getAlignedSize() const -> VkDeviceSize;
  [[nodiscard]] auto getBindingOffset(uint32_t binding) const -> VkDeviceSize;

private:
  Device& device;
  VkDescriptorSetLayout vkLayout;
};

}
