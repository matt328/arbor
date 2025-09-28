#pragma once

#include <memory>
#include <string>
#include <unordered_map>

namespace arb {

class IDispatcher;

struct DispatcherBinding {
  std::shared_ptr<IDispatcher> dispatcher;
  std::unordered_map<std::string, std::string> bindingMap;
};

}
