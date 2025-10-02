#pragma once

#include <vulkan/vulkan.h>
#include <optional>
#include <string>

#include "DebugUtils.hpp"
#include "ErrorUtils.hpp"
#include "vulkan/vulkan_core.h"

namespace arb {

struct Semaphore {
  VkDevice device{VK_NULL_HANDLE};
  VkSemaphore handle{VK_NULL_HANDLE};

  Semaphore() = default;

  explicit Semaphore(VkDevice d,
                     bool timeline = false,
                     std::optional<std::string> name = std::nullopt)
      : device(d) {
    auto createInfo = VkSemaphoreCreateInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    if (timeline) {
      const auto typeCreateInfo =
          VkSemaphoreTypeCreateInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
                                    .pNext = nullptr,
                                    .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
                                    .initialValue = 0};
      createInfo.pNext = &typeCreateInfo;
    }
    checkVk(vkCreateSemaphore(device, &createInfo, nullptr, &handle), "vkCreateSemaphore");
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
