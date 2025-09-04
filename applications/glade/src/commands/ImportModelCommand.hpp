#pragma once

#include "adapters/AssetAdapter.hpp"

namespace gld {

class ImportModelCommand : public QUndoCommand {
public:
  ImportModelCommand(AssetAdapter* newAdapter,
                     ImportModelDetails newDetails,
                     QUndoCommand* parent = nullptr)
      : QUndoCommand{"Import Model", parent}, adapter{newAdapter}, details{std::move(newDetails)} {
  }

  auto redo() -> void override {
    adapter->importModel(details);
  }

  auto undo() -> void override {
    adapter->removeModel(importedId);
  }

private:
  AssetAdapter* adapter;
  ImportModelDetails details;
  uint32_t importedId{};
};

}
