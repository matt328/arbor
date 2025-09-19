#include "DispatcherFactory.hpp"

#include <cassert>
#include <variant>

#include "bk/Logger.hpp"
#include "resources/ResourceFacade.hpp"

namespace arb {

DispatcherFactory::DispatcherFactory(ResourceFacade& newResourceFacade)
    : resourceFacade{newResourceFacade} {
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
        dispatcherMap.emplace(handle, info.create(resourceFacade));
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
