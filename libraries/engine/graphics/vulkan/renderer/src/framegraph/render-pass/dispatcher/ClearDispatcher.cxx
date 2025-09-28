#include "ClearDispatcher.hpp"

#include "IDispatcher.hpp"
#include "LogicalImageRequirement.hpp"
#include "bk/Logger.hpp"

namespace arb {

ClearDispatcher::ClearDispatcher(AliasRegistry& newAliasRegistry) : IDispatcher(newAliasRegistry) {
  Log::trace("Creating ClearDispatcher");
}

ClearDispatcher::~ClearDispatcher() {
  Log::trace("Destroying ClearDispatcher");
}

auto ClearDispatcher::dispatch(const Frame* frame,
                               VkPipelineLayout pipelineLayout,
                               VkCommandBuffer commandBuffer) -> void {
}

auto ClearDispatcher::getPushConstantSize() -> size_t {
}

void ClearDispatcher::bindAlias(std::string alias, std::string logicalName) {
  logicalToAliasMap.emplace(logicalName, alias);
}

auto ClearDispatcher::requirements() -> DispatcherRequirements {
  return DispatcherRequirements{
      .images = {LogicalImageRequirement{.name = "ColorTarget",
                                         .isGlobal = false,
                                         .accessType = AliasAccess::Write,
                                         .usageType = AliasUseType::Attachment,
                                         .format = VK_FORMAT_R8G8B8A8_UNORM,
                                         .extent = {},
                                         .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT},
                 LogicalImageRequirement{.name = "DepthTarget",
                                         .isGlobal = false,
                                         .accessType = AliasAccess::Write,
                                         .usageType = AliasUseType::Attachment,
                                         .format = VK_FORMAT_D32_SFLOAT,
                                         .extent = {},
                                         .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT}}};
}

}
