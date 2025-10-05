#include "core/shader-binding/DSLayoutManager.hpp"

#include <cassert>

#include "bk/Log.hpp"
#include "core/Device.hpp"
#include "vulkan/vulkan_core.h"

namespace arb {

DSLayoutManager::DSLayoutManager(Device& newDevice) : device{newDevice} {
  LOG_TRACE_L1(Log::Core, "Creating DSLayoutManager");
}

DSLayoutManager::~DSLayoutManager() {
  LOG_TRACE_L1(Log::Core, "Destroying DSLayoutManager");
}

auto DSLayoutManager::createLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings,
                                   std::optional<std::string> name) -> DSLayoutHandle {
  const auto handle = handleGenerator.requestHandle();
  const auto info =
      VkDescriptorSetLayoutCreateInfo{.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                                      .bindingCount = static_cast<uint32_t>(bindings.size()),
                                      .pBindings = bindings.data()};
  layoutMap.emplace(handle,
                    std::make_unique<DSLayout>(device, info, name.value_or("Unnamed Layout")));
  return handle;
}

auto DSLayoutManager::getDSLayout(DSLayoutHandle handle) -> DSLayout& {
  assert(layoutMap.contains(handle));
  return *layoutMap.at(handle);
}

}
