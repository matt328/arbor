#pragma once

#include "bk/NonCopyMove.hpp"
#include "PassGraphInfo.hpp"

namespace arb {

class IGraphInfoProvider : NonCopyableMovable {
public:
  IGraphInfoProvider() = default;
  virtual ~IGraphInfoProvider() = default;

  [[nodiscard]] virtual auto getGraphInfo() const -> PassGraphInfo = 0;
};

}
