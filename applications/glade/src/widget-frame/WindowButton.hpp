#pragma once

namespace QWK {

class WindowButtonPrivate;

class WindowButton : public QPushButton {
  Q_OBJECT
  Q_DECLARE_PRIVATE(WindowButton)
  Q_PROPERTY(QIcon iconNormal READ iconNormal WRITE setIconNormal FINAL)
  Q_PROPERTY(QIcon iconChecked READ iconChecked WRITE setIconChecked FINAL)
  Q_PROPERTY(QIcon iconDisabled READ iconDisabled WRITE setIconDisabled FINAL)
public:
  explicit WindowButton(QWidget* parent = nullptr);
  ~WindowButton();

  [[nodiscard]] auto iconNormal() const -> QIcon;
  void setIconNormal(const QIcon& icon);

  [[nodiscard]] auto iconChecked() const -> QIcon;
  void setIconChecked(const QIcon& icon);

  [[nodiscard]] auto iconDisabled() const -> QIcon;
  void setIconDisabled(const QIcon& icon);

Q_SIGNALS:
  void doubleClicked();

protected:
  void checkStateSet() override;

  void mouseDoubleClickEvent(QMouseEvent* event) override;

  WindowButton(WindowButtonPrivate& d, QWidget* parent = nullptr);

  QScopedPointer<WindowButtonPrivate> d_ptr;
};

}
