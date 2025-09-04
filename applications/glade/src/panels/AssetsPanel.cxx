#include "AssetsPanel.hpp"

namespace gld {

AssetsPanel::AssetsPanel(ActionManager::AssetActions actions, QWidget* parent) : QWidget(parent) {
  m_toolbar = new QToolBar(this);
  m_toolbar->setStyleSheet("QToolBar { padding-bottom: 0px; } "
                           "QToolButton { margin-bottom: 0px; }");
  m_toolbar->addAction(actions.importModel);
  m_toolbar->addAction(actions.importAnimation);
  m_toolbar->addAction(actions.importSkeleton);

  m_treeView = new QTreeView(this);
  m_treeView->setUniformRowHeights(true);

  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(m_toolbar);
  layout->addWidget(m_treeView);
}

}
