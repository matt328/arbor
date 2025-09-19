#pragma once

#include "CullingCreateInfo.hpp"
#include "framegraph/ComponentIds.hpp"
#include "bk/NonCopyMove.hpp"

#include <variant>

namespace arb {

class IRenderPass;

using PassInfo = std::variant<CullingCreateInfo>;

struct PassCreateInfo {
  PassId passId;
  PassInfo passInfo;
};

class PassFactory : public NonCopyableMovable {
public:
  PassFactory();
  ~PassFactory();

  auto createPass(const PassCreateInfo& createInfo) -> std::unique_ptr<IRenderPass>;
};

}
