#pragma once

namespace gld {

class ProjectService;

class AssetTreeModel : public QAbstractItemModel {
public:
  explicit AssetTreeModel(ProjectService& newProjectService, QObject* parent = nullptr);

  [[nodiscard]] auto index(int row, int column, const QModelIndex& parent) const
      -> QModelIndex override;
  [[nodiscard]] auto parent(const QModelIndex& child) const -> QModelIndex override;
  [[nodiscard]] auto rowCount(const QModelIndex& parent) const -> int override;
  [[nodiscard]] auto columnCount(const QModelIndex& parent) const -> int override;
  [[nodiscard]] auto data(const QModelIndex& index, int role) const -> QVariant override;

private:
  ProjectService& projectService;
};

}
