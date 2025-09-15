#pragma once

#include "framegraph/PassGraphInfo.hpp"

namespace arb {

class IGraphInfoProvider {
public:
  IGraphInfoProvider() = default;
  virtual ~IGraphInfoProvider() = default;

  IGraphInfoProvider(const IGraphInfoProvider&) = default;
  IGraphInfoProvider(IGraphInfoProvider&&) = delete;
  auto operator=(const IGraphInfoProvider&) -> IGraphInfoProvider& = default;
  auto operator=(IGraphInfoProvider&&) -> IGraphInfoProvider& = delete;

  [[nodiscard]] virtual auto getGraphInfo() const -> PassGraphInfo = 0;
};

}
