#include "PassFactory.hpp"

#include "bk/Logger.hpp"
#include "framegraph/render-pass/IRenderPass.hpp"

namespace arb {

PassFactory::PassFactory() {
  Log::trace("Creating PassFactory");
}

PassFactory::~PassFactory() {
  Log::trace("Destroying PassFactory");
}

auto PassFactory::createPass(const PassCreateInfo& createInfo) -> std::unique_ptr<IRenderPass> {
  const auto visitor = [&](auto&& arg) -> std::unique_ptr<IRenderPass> {
    using T = std::decay_t<decltype(arg)>;
    if constexpr (std::is_same_v<T, ForwardCreateInfo>) {
      return createForwardPass(createInfo.passId, arg);
    }
    if constexpr (std::is_same_v<T, CullingCreateInfo>) {
      return createCullingPass(createInfo.passId, arg);
    }
    if constexpr (std::is_same_v<T, CompositionCreateInfo>) {
      return createCompositionPass(createInfo.passId, arg);
    }
    return nullptr;
  };
  return std::visit(visitor, createInfo.passInfo);
}

auto PassFactory::createForwardPass(PassId passId, const ForwardCreateInfo& createInfo)
    -> std::unique_ptr<IRenderPass> {
}

auto PassFactory::createCullingPass(PassId passId, CullingCreateInfo createInfo)
    -> std::unique_ptr<IRenderPass> {
}

auto PassFactory::createCompositionPass(PassId passId, CompositionCreateInfo createInfo)
    -> std::unique_ptr<IRenderPass> {
}

}
