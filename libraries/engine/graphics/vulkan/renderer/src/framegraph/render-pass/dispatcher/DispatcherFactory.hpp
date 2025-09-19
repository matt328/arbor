#pragma once

#include <variant>

#include "CompositionCreateInfo.hpp"
#include "CullingCreateInfo.hpp"
#include "DispatcherHandles.hpp"
#include "ForwardCreateInfo.hpp"
#include "IDispatcher.hpp"
#include "engine/common/HandleGenerator.hpp"

namespace arb {

using DispatcherCreateInfo =
    std::variant<CompositionCreateInfo, CullingCreateInfo, ForwardCreateInfo>;

class DispatcherFactory {
public:
  DispatcherFactory(ResourceFacade& resourceFacade);
  ~DispatcherFactory();

  auto createDispatcher(const DispatcherCreateInfo& createInfo) -> DispatcherHandle;

  auto getDispatcherHandle(DispatcherId id) -> DispatcherHandle;

  auto getDispatcher(DispatcherHandle handle) -> IDispatcher&;

private:
  ResourceFacade& resourceFacade;

  HandleGenerator<DispatcherTag> handleGenerator;
  std::unordered_map<DispatcherHandle, std::unique_ptr<IDispatcher>> dispatcherMap;
  std::unordered_map<DispatcherId, DispatcherHandle> handlesById;
};

}
