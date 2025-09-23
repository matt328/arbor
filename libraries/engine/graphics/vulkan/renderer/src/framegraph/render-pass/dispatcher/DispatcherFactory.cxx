#include "DispatcherFactory.hpp"

#include <cassert>
#include <variant>

#include "bk/Logger.hpp"
#include "framegraph/render-pass/dispatcher/CullingCreateInfo.hpp"
#include "framegraph/render-pass/dispatcher/IDispatcher.hpp"
#include "resources/ResourceSystem.hpp"

namespace arb {

DispatcherFactory::DispatcherFactory(ResourceSystem& newResourceSystem)
    : resourceSystem{newResourceSystem} {
  Log::trace("Creating DispatcherFactory");
}

DispatcherFactory::~DispatcherFactory() {
  Log::trace("Destroying DispatcherFactory");
}

auto DispatcherFactory::createDispatcher(const DispatcherCreateInfo& createInfo)
    -> DispatcherHandle {
  const auto handle = handleGenerator.requestHandle();

  std::visit(
      [&](auto&& info) -> void {
        Log::trace("visiting");
        handlesById.emplace(info.id, handle);
        auto dispatcher = info.create(resourceSystem);
        dispatcherMap.emplace(handle, info.create(resourceSystem));
      },
      createInfo);

  return handle;
}

auto DispatcherFactory::getDispatcherHandle(DispatcherId id) -> DispatcherHandle {
  assert(handlesById.contains(id));
  return handlesById.at(id);
}

auto DispatcherFactory::getDispatcher(DispatcherHandle handle) -> IDispatcher& {
  assert(dispatcherMap.contains(handle));
  return *dispatcherMap.at(handle);
}

}
