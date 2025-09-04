#pragma once

namespace gld {

class ActionController : public QObject {
  Q_OBJECT
public:
  ActionController(QObject* parent);
  ~ActionController() = default;

  ActionController(const ActionController&) = delete;
  ActionController(ActionController&&) = delete;
  auto operator=(const ActionController&) -> ActionController& = delete;
  auto operator=(ActionController&&) -> ActionController& = delete;

  using QObject::QObject;

signals:
  void importAsset();
};

}
