#include "AssetTreeModel.hpp"

namespace gld {

AssetTreeModel::AssetTreeModel(ProjectService& newProjectService, QObject* parent)
    : QAbstractItemModel(parent), projectService(newProjectService) {
}

auto AssetTreeModel::index(int row, int column, const QModelIndex& parent) const -> QModelIndex {
  return QModelIndex{};
}

auto AssetTreeModel::parent(const QModelIndex& child) const -> QModelIndex {
  return QModelIndex{};
}

auto AssetTreeModel::rowCount(const QModelIndex& parent) const -> int {
  return 0;
}

auto AssetTreeModel::columnCount(const QModelIndex& parent) const -> int {
  return 1;
}

auto AssetTreeModel::data(const QModelIndex& index, int role) const -> QVariant {
  return {};
}

}
