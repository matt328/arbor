#pragma once

#include "DebugUtils.hpp"

namespace arb {

struct Semaphore {
  VkDevice device{VK_NULL_HANDLE};
  VkSemaphore handle{VK_NULL_HANDLE};

  Semaphore() = default;

  explicit Semaphore(VkDevice d, std::optional<std::string> name = std::nullopt) : device(d) {
    VkSemaphoreCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };
    if (vkCreateSemaphore(device, &createInfo, nullptr, &handle) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create semaphore");
    }
    if (name) {
      dbg::setDebugName(device, handle, *name);
    }
  }

  ~Semaphore() {
    if (handle != VK_NULL_HANDLE) {
      vkDestroySemaphore(device, handle, nullptr);
    }
  }

  Semaphore(const Semaphore&) = delete;
  auto operator=(const Semaphore&) -> Semaphore& = delete;

  Semaphore(Semaphore&& other) noexcept : device(other.device), handle(other.handle) {
    other.handle = VK_NULL_HANDLE;
    other.device = VK_NULL_HANDLE;
  }

  auto operator=(Semaphore&& other) noexcept -> Semaphore& {
    if (this != &other) {
      cleanup();
      device = other.device;
      handle = other.handle;
      other.handle = VK_NULL_HANDLE;
      other.device = VK_NULL_HANDLE;
    }
    return *this;
  }

  operator VkSemaphore() const {
    return handle;
  }

private:
  void cleanup() {
    if (handle != VK_NULL_HANDLE) {
      vkDestroySemaphore(device, handle, nullptr);
      handle = VK_NULL_HANDLE;
    }
  }
};

}
