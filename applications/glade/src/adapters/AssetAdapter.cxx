#include "AssetAdapter.hpp"

#include "bk/Log.hpp"

#include "ImportModelDetails.hpp"

namespace gld {

AssetAdapter::AssetAdapter(std::shared_ptr<bk::IEventQueue> newEventQueue, QObject* parent)
    : eventQueue{std::move(newEventQueue)}, QObject{parent} {
}

auto AssetAdapter::importModel(const ImportModelDetails& details) -> void {
  LOG_TRACE_L1(Log::Assets,
               "adapter importing model details: filename={}, id={}",
               details.filename.toStdString(),
               details.identifier.toStdString());
}

auto AssetAdapter::removeModel(uint32_t id) -> void {
  LOG_TRACE_L1(Log::Assets, "adapter removing model with id={}", id);
}

}
