#include "core/shader-binding/DSLayout.hpp"

#include "bk/Logger.hpp"
#include "common/ErrorUtils.hpp"
#include "common/DebugUtils.hpp"

namespace arb {

/*
  TODO: Separate shaderbindingfactory in core from the creation/management of
  ShaderBindingProxies in the renderer. ShaderBindingProxy was a heavyweight logical representation
  of per-frame shaderbindings. However buffermanager handles this, ShaderBindings should be the
  same. BufferManager externalizes the frame handling, and just provides a method for registering
  multiple buffers at a time given a number of frames, then returns the LogicalHandle with a vector
  of (physical)Handles so callers can do whatever they need with them, probably register them with
  the FrameManager. I think this functionality can live in Core as it doesn't introduce a dependency
  on FrameManager, but still centralizes ShaderBinding creation and ownership. I think instead of
  handing out heavyweight shaderbindingproxy objects, maybe the ShaderBindingManager should provide
  methods to operate on LogicalSBHandles. When updating a LogicalShaderBindingHandle, should queue
  up writes at that time to all the ShaderBindings, but defer applying them until it's bound to the
  pipeline

  Can 'update' ShaderBindings at any time, but must queue the updates so they're only processed
  before calling cmd.bindDescriptorSets. Can't just updateDescriptorSets any time
*/

DSLayout::DSLayout(Device& newDevice,
                   const VkDescriptorSetLayoutCreateInfo& info,
                   std::string_view name)
    : device{newDevice}, vkLayout{VK_NULL_HANDLE} {
  Log::trace("Creating DSLayout: {}", name);
  checkVk(vkCreateDescriptorSetLayout(device, &info, nullptr, &vkLayout),
          "vkCreateDescriptorSetLayout");
  dbg::setDebugName(device, vkLayout, name);
}

DSLayout::~DSLayout() noexcept {
  Log::trace("Destroying DSLayout");
  vkDestroyDescriptorSetLayout(device, vkLayout, nullptr);
}

auto DSLayout::getLayoutSize() const -> VkDeviceSize {
  auto size = VkDeviceSize{};
  vkGetDescriptorSetLayoutSizeEXT(device, vkLayout, &size);
  return size;
}

auto DSLayout::getAlignedSize() const -> VkDeviceSize {
  const auto alignment = 0;
  // TODO: vkDeviceGetDescriptorBufferProperties().descriptorBufferOffsetAlignment()
  // or something like that when using descriptor buffers
  const auto size = getLayoutSize();
  return (size + alignment - 1) & ~(alignment - 1);
}

auto DSLayout::getBindingOffset(const uint32_t binding) const -> VkDeviceSize {
  auto offset = VkDeviceSize{};
  vkGetDescriptorSetLayoutBindingOffsetEXT(device, vkLayout, binding, &offset);
  return offset;
}

}
