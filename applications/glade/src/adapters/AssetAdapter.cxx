#include "AssetAdapter.hpp"
#include "ImportModelDetails.hpp"

namespace gld {

AssetAdapter::AssetAdapter(std::shared_ptr<bk::IEventQueue> newEventQueue, QObject* parent)
    : eventQueue{std::move(newEventQueue)}, QObject{parent} {
}

auto AssetAdapter::importModel(const ImportModelDetails& details) -> void {
  Log->trace("adapter importing model details: filename={}, id={}",
             details.filename.toStdString(),
             details.identifier.toStdString());
}

auto AssetAdapter::removeModel(uint32_t id) -> void {
  Log->trace("adapter removing model with id={}", id);
}

}
