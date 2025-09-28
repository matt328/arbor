#pragma once

#include <optional>
#include <string>

#include "common/ErrorUtils.hpp"
#include "DebugUtils.hpp"

namespace arb {

struct Fence {
  VkDevice device{VK_NULL_HANDLE};
  VkFence handle{VK_NULL_HANDLE};

  Fence() = default;

  explicit Fence(VkDevice d, std::optional<std::string> name = std::nullopt) : device(d) {
    VkFenceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    };
    checkVk(vkCreateFence(device, &createInfo, nullptr, &handle), "vkCreateFence");

    if (name) {
      dbg::setDebugName(device, handle, *name);
    }
  }

  ~Fence() {
    if (handle != VK_NULL_HANDLE) {
      vkDestroyFence(device, handle, nullptr);
    }
  }

  Fence(const Fence&) = delete;
  auto operator=(const Fence&) -> Fence& = delete;

  Fence(Fence&& other) noexcept : device(other.device), handle(other.handle) {
    other.handle = VK_NULL_HANDLE;
    other.device = VK_NULL_HANDLE;
  }

  auto operator=(Fence&& other) noexcept -> Fence& {
    if (this != &other) {
      cleanup();
      device = other.device;
      handle = other.handle;
      other.handle = VK_NULL_HANDLE;
      other.device = VK_NULL_HANDLE;
    }
    return *this;
  }

  operator VkFence() const {
    return handle;
  }

private:
  void cleanup() {
    if (handle != VK_NULL_HANDLE) {
      vkDestroyFence(device, handle, nullptr);
      handle = VK_NULL_HANDLE;
    }
  }
};

}
