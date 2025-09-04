#pragma once

namespace gld {

class ActionManager : QObject {
public:
  struct AssetActions {
    QAction* importModel;
    QAction* importAnimation;
    QAction* importSkeleton;
  };

  explicit ActionManager(QObject* parent) : QObject{parent} {
    // Asset
    assetActions.importModel = new QAction(QIcon(":/app/model.svg"), tr("Import Model"), this);
    assetActions.importAnimation =
        new QAction(QIcon(":/app/animation.svg"), tr("Import Animation"), this);
    assetActions.importSkeleton =
        new QAction(QIcon(":/app/skeleton.svg"), tr("Import Skeleton"), this);
  }

  AssetActions assetActions;
};

}
