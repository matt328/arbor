#pragma once

#include "actions/ActionManager.hpp"

namespace gld {

class AssetsPanel : public QWidget {
  Q_OBJECT
public:
  explicit AssetsPanel(ActionManager::AssetActions actions, QWidget* parent = nullptr);
  [[nodiscard]] auto treeView() const -> QTreeView*;
  [[nodiscard]] auto toolbar() const -> QToolBar*;

signals:
  void importAssets();
  void refreshAssets();

private:
  QTreeView* m_treeView;
  QToolBar* m_toolbar;
};

}
