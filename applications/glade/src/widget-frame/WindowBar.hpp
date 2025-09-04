#pragma once

namespace QWK {

class WindowBarPrivate;

class WindowBar : public QFrame {
  Q_OBJECT
  Q_DECLARE_PRIVATE(WindowBar)
public:
  explicit WindowBar(QWidget* parent = nullptr);
  ~WindowBar();

  [[nodiscard]] auto menuBar() const -> QMenuBar*;
  [[nodiscard]] auto titleLabel() const -> QLabel*;
  [[nodiscard]] auto iconButton() const -> QAbstractButton*;
  [[nodiscard]] auto minButton() const -> QAbstractButton*;
  [[nodiscard]] auto maxButton() const -> QAbstractButton*;
  [[nodiscard]] auto closeButton() const -> QAbstractButton*;

  void setMenuBar(QMenuBar* menuBar);
  void setTitleLabel(QLabel* label);
  void setIconButton(QAbstractButton* btn);
  void setMinButton(QAbstractButton* btn);
  void setMaxButton(QAbstractButton* btn);
  void setCloseButton(QAbstractButton* btn);

  auto takeMenuBar() -> QMenuBar*;
  auto takeTitleLabel() -> QLabel*;
  auto takeIconButton() -> QAbstractButton*;
  auto takeMinButton() -> QAbstractButton*;
  auto takeMaxButton() -> QAbstractButton*;
  auto takeCloseButton() -> QAbstractButton*;

  [[nodiscard]] auto hostWidget() const -> QWidget*;
  void setHostWidget(QWidget* w);

  [[nodiscard]] auto titleFollowWindow() const -> bool;
  void setTitleFollowWindow(bool value);

  [[nodiscard]] auto iconFollowWindow() const -> bool;
  void setIconFollowWindow(bool value);

Q_SIGNALS:
  void minimizeRequested();
  void maximizeRequested(bool max = false);
  void closeRequested();

protected:
  auto eventFilter(QObject* obj, QEvent* event) -> bool override;

  virtual void titleChanged(const QString& text);
  virtual void iconChanged(const QIcon& icon);

  WindowBar(WindowBarPrivate& d, QWidget* parent = nullptr);

  QScopedPointer<WindowBarPrivate> d_ptr;
};

}
