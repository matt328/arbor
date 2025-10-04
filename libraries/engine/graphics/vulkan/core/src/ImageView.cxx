#include "core/ImageView.hpp"

#include "bk/Logger.hpp"
#include "common/ErrorUtils.hpp"
#include "common/DebugUtils.hpp"
#include "core/Device.hpp"
#include "vulkan/vulkan_core.h"

namespace arb {

ImageView::ImageView(Device& newDevice,
                     const VkImageViewCreateInfo& createInfo,
                     ImageHandle imageHandle,
                     const std::optional<std::string>& name)
    : device{newDevice},
      handle{VK_NULL_HANDLE},
      sourceImage{imageHandle},
      debugName{name.value_or("Unnamed ImageView")} {
  Log::trace("Creating ImageView {}", debugName);
  checkVk(vkCreateImageView(device, &createInfo, nullptr, &handle), "vkCreateImageView");
  dbg::setDebugName(device, handle, debugName);
}

ImageView::~ImageView() {
  Log::trace("Destroying ImageView {}", debugName);
  if (handle != VK_NULL_HANDLE) {
    vkDestroyImageView(device, handle, nullptr);
    handle = VK_NULL_HANDLE;
  }
}

}
