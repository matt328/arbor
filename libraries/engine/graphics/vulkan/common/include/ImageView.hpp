#pragma once

#include "DebugUtils.hpp"

namespace arb {

struct ImageView {
  VkDevice device{VK_NULL_HANDLE};
  VkImageView view{VK_NULL_HANDLE};

  ImageView() = default;

  ImageView(VkDevice d,
            const VkImageViewCreateInfo& createInfo,
            std::optional<std::string> name = std::nullopt)
      : device(d) {
    if (vkCreateImageView(device, &createInfo, nullptr, &view) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create VkImageView");
    }
    if (name) {
      dbg::setDebugName(device, view, *name);
    }
  }

  ~ImageView() {
    if (view != VK_NULL_HANDLE) {
      vkDestroyImageView(device, view, nullptr);
    }
  }

  // non-copyable
  ImageView(const ImageView&) = delete;
  auto operator=(const ImageView&) -> ImageView& = delete;

  // movable
  ImageView(ImageView&& other) noexcept : device(other.device), view(other.view) {
    other.view = VK_NULL_HANDLE;
    other.device = VK_NULL_HANDLE;
  }

  auto operator=(ImageView&& other) noexcept -> ImageView& {
    if (this != &other) {
      cleanup();
      device = other.device;
      view = other.view;
      other.view = VK_NULL_HANDLE;
      other.device = VK_NULL_HANDLE;
    }
    return *this;
  }

  operator VkImageView() const {
    return view;
  }

private:
  void cleanup() {
    if (view != VK_NULL_HANDLE) {
      vkDestroyImageView(device, view, nullptr);
      view = VK_NULL_HANDLE;
    }
  }
};

}
