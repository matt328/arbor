#include "core/ImageView.hpp"

#include "bk/Log.hpp"
#include "common/ErrorUtils.hpp"
#include "common/DebugUtils.hpp"
#include "core/Device.hpp"
#include "vulkan/vulkan_core.h"
#include "core/Image.hpp"

namespace arb {

ImageView::ImageView(Device& newDevice,
                     const VkImageViewCreateInfo& newCreateInfo,
                     ImageHandle imageHandle,
                     const std::optional<std::string>& name)
    : device{newDevice},
      handle{VK_NULL_HANDLE},
      sourceImage{imageHandle},
      createInfo{newCreateInfo},
      debugName{name.value_or("Unnamed ImageView")} {
  LOG_TRACE_L1(Log::Core, "Creating ImageView {}", debugName);
  checkVk(vkCreateImageView(device, &createInfo, nullptr, &handle), "vkCreateImageView");
  dbg::setDebugName(device, handle, debugName);
}

void ImageView::recreate(Image& image) {
  auto newCreateInfo = createInfo;
  if (handle != VK_NULL_HANDLE) {
    vkDestroyImageView(device, handle, nullptr);
  }
  newCreateInfo.image = image;
  checkVk(vkCreateImageView(device, &newCreateInfo, nullptr, &handle), "vkCreateImageView");
  dbg::setDebugName(device, handle, debugName);
}

ImageView::~ImageView() {
  LOG_TRACE_L1(Log::Core, "Destroying ImageView {}", debugName);
  if (handle != VK_NULL_HANDLE) {
    vkDestroyImageView(device, handle, nullptr);
    handle = VK_NULL_HANDLE;
  }
}

}
