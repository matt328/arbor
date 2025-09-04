#pragma once

#include "ImportModelDetails.hpp"
#include "IEventQueue.hpp"

namespace gld {

class AssetAdapter : public QObject {
  Q_OBJECT
public:
  AssetAdapter(std::shared_ptr<arb::IEventQueue> newEventQueue, QObject* parent = nullptr);

  auto model() -> QAbstractItemModel*;

  using QObject::QObject;

  auto removeModel(uint32_t id) -> void;
  void importModel(const ImportModelDetails& details);

private:
  std::shared_ptr<arb::IEventQueue> eventQueue;
};

}
