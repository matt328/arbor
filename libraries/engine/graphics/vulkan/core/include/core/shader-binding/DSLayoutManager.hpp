#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

#include "DSLayout.hpp"
#include "bk/NonCopyMove.hpp"
#include "Handles.hpp"
#include "engine/common/HandleGenerator.hpp"

namespace arb {

class DSLayoutManager : NonCopyableMovable {
public:
  explicit DSLayoutManager(Device& newDevice);
  ~DSLayoutManager();

  auto createLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings,
                    std::optional<std::string> name = std::nullopt) -> DSLayoutHandle;

  auto getDSLayout(DSLayoutHandle handle) -> DSLayout&;

private:
  Device& device;
  HandleGenerator<DSLayoutTag> handleGenerator;
  std::unordered_map<DSLayoutHandle, std::unique_ptr<DSLayout>> layoutMap;
};

}
