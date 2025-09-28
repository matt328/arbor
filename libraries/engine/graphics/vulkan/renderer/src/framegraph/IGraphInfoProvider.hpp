#pragma once

#include "bk/NonCopyMove.hpp"
#include "framegraph/render-pass/pass/reqs/PassDescription.hpp"

namespace arb {

class IGraphInfoProvider : NonCopyableMovable {
public:
  IGraphInfoProvider() = default;
  virtual ~IGraphInfoProvider() = default;

  [[nodiscard]] virtual auto getDescription() const -> PassDescription = 0;
};

}
