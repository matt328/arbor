#pragma once

#include <variant>

#include "CompositionCreateInfo.hpp"
#include "CullingCreateInfo.hpp"
#include "ForwardCreateInfo.hpp"
#include "framegraph/ComponentIds.hpp"
#include "bk/NonCopyMove.hpp"

namespace arb {

class IRenderPass;

using PassInfo = std::variant<CullingCreateInfo, ForwardCreateInfo, CompositionCreateInfo>;

struct PassCreateInfo {
  PassId passId{};
  PassInfo passInfo;
};

class PassFactory : public NonCopyableMovable {
public:
  PassFactory();
  ~PassFactory();

  auto createPass(const PassCreateInfo& createInfo) -> std::unique_ptr<IRenderPass>;

private:
  auto createForwardPass(PassId passId, const ForwardCreateInfo& createInfo)
      -> std::unique_ptr<IRenderPass>;
  auto createCullingPass(PassId passId, CullingCreateInfo createInfo)
      -> std::unique_ptr<IRenderPass>;
  auto createCompositionPass(PassId passId, CompositionCreateInfo createInfo)
      -> std::unique_ptr<IRenderPass>;
};

}
