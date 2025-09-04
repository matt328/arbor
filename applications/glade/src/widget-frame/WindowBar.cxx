#include "WindowBar.hpp"
#include "WindowBarPrivate.hpp"

namespace QWK {

WindowBarPrivate::WindowBarPrivate() {
  w = nullptr;
  autoTitle = true;
  autoIcon = false;
}

WindowBarPrivate::~WindowBarPrivate() = default;

void WindowBarPrivate::init() {
  Q_Q(WindowBar);
  layout = new QHBoxLayout();
  if (QLocale::system().textDirection() == Qt::RightToLeft) {
    layout->setDirection(QBoxLayout::RightToLeft);
  }

  layout->setContentsMargins(QMargins());
  layout->setSpacing(0);
  for (int i = IconButton; i <= CloseButton; ++i) {
    insertDefaultSpace(i);
  }
  q->setLayout(layout);
}

void WindowBarPrivate::setWidgetAt(int index, QWidget* widget) const {
  auto* item = layout->takeAt(index);
  auto* orgWidget = item->widget();
  if (orgWidget != nullptr) {
    orgWidget->deleteLater();
  }
  delete item;
  if (widget == nullptr) {
    insertDefaultSpace(index);
  } else {
    layout->insertWidget(index, widget);
  }
}

auto WindowBarPrivate::takeWidgetAt(int index) const -> QWidget* {
  auto* item = layout->itemAt(index);
  auto* orgWidget = item->widget();
  if (orgWidget != nullptr) {
    item = layout->takeAt(index);
    delete item;
    insertDefaultSpace(index);
  }
  return orgWidget;
}

WindowBar::WindowBar(QWidget* parent) : WindowBar(*new WindowBarPrivate(), parent) {
}

WindowBar::~WindowBar() = default;

auto WindowBar::menuBar() const -> QMenuBar* {
  Q_D(const WindowBar);
  return static_cast<QMenuBar*>(d->widgetAt(WindowBarPrivate::MenuWidget));
}

auto WindowBar::titleLabel() const -> QLabel* {
  Q_D(const WindowBar);
  return static_cast<QLabel*>(d->widgetAt(WindowBarPrivate::TitleLabel));
}

auto WindowBar::iconButton() const -> QAbstractButton* {
  Q_D(const WindowBar);
  return static_cast<QAbstractButton*>(d->widgetAt(WindowBarPrivate::IconButton));
}

auto WindowBar::minButton() const -> QAbstractButton* {
  Q_D(const WindowBar);
  return static_cast<QAbstractButton*>(d->widgetAt(WindowBarPrivate::MinimizeButton));
}

auto WindowBar::maxButton() const -> QAbstractButton* {
  Q_D(const WindowBar);
  return static_cast<QAbstractButton*>(d->widgetAt(WindowBarPrivate::MaximizeButton));
}

auto WindowBar::closeButton() const -> QAbstractButton* {
  Q_D(const WindowBar);
  return static_cast<QAbstractButton*>(d->widgetAt(WindowBarPrivate::CloseButton));
}

void WindowBar::setMenuBar(QMenuBar* menuBar) {
  Q_D(WindowBar);
  auto* org = takeMenuBar();
  if (org != nullptr) {
    org->deleteLater();
  }
  if (menuBar == nullptr) {
    return;
  }
  d->setWidgetAt(WindowBarPrivate::MenuWidget, menuBar);
  menuBar->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
}

void WindowBar::setTitleLabel(QLabel* label) {
  Q_D(WindowBar);
  auto* org = takeTitleLabel();
  if (org != nullptr) {
    org->deleteLater();
  }
  if (label == nullptr) {
    return;
  }
  d->setWidgetAt(WindowBarPrivate::TitleLabel, label);
  if (d->autoTitle && (d->w != nullptr)) {
    label->setText(d->w->windowTitle());
  }
  label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
}

void WindowBar::setIconButton(QAbstractButton* btn) {
  Q_D(WindowBar);
  auto* org = takeIconButton();
  if (org != nullptr) {
    org->deleteLater();
  }
  if (btn == nullptr) {
    return;
  }
  d->setWidgetAt(WindowBarPrivate::IconButton, btn);
  if (d->autoIcon && (d->w != nullptr)) {
    btn->setIcon(d->w->windowIcon());
  }
  btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
}

void WindowBar::setMinButton(QAbstractButton* btn) {
  Q_D(WindowBar);
  auto* org = takeMinButton();
  if (org != nullptr) {
    org->deleteLater();
  }
  if (btn == nullptr) {
    return;
  }
  d->setWidgetAt(WindowBarPrivate::MinimizeButton, btn);
  connect(btn, &QAbstractButton::clicked, this, &WindowBar::minimizeRequested);
}

void WindowBar::setMaxButton(QAbstractButton* btn) {
  Q_D(WindowBar);
  auto* org = takeMaxButton();
  if (org != nullptr) {
    org->deleteLater();
  }
  if (btn == nullptr) {
    return;
  }
  d->setWidgetAt(WindowBarPrivate::MaximizeButton, btn);
  connect(btn, &QAbstractButton::clicked, this, &WindowBar::maximizeRequested);
}

void WindowBar::setCloseButton(QAbstractButton* btn) {
  Q_D(WindowBar);
  auto* org = takeCloseButton();
  if (org != nullptr) {
    org->deleteLater();
  }
  if (btn == nullptr) {
    return;
  }
  d->setWidgetAt(WindowBarPrivate::CloseButton, btn);
  connect(btn, &QAbstractButton::clicked, this, &WindowBar::closeRequested);
}

auto WindowBar::takeMenuBar() -> QMenuBar* {
  Q_D(WindowBar);
  return static_cast<QMenuBar*>(d->takeWidgetAt(WindowBarPrivate::MenuWidget));
}

auto WindowBar::takeTitleLabel() -> QLabel* {
  Q_D(WindowBar);
  return static_cast<QLabel*>(d->takeWidgetAt(WindowBarPrivate::TitleLabel));
}

auto WindowBar::takeIconButton() -> QAbstractButton* {
  Q_D(WindowBar);
  return static_cast<QAbstractButton*>(d->takeWidgetAt(WindowBarPrivate::IconButton));
}

auto WindowBar::takeMinButton() -> QAbstractButton* {
  Q_D(WindowBar);
  auto* btn = static_cast<QAbstractButton*>(d->takeWidgetAt(WindowBarPrivate::MinimizeButton));
  if (btn == nullptr) {
    return nullptr;
  }
  disconnect(btn, &QAbstractButton::clicked, this, &WindowBar::minimizeRequested);
  return btn;
}

QAbstractButton* WindowBar::takeMaxButton() {
  Q_D(WindowBar);
  auto* btn = static_cast<QAbstractButton*>(d->takeWidgetAt(WindowBarPrivate::MaximizeButton));
  if (btn == nullptr) {
    return nullptr;
  }
  disconnect(btn, &QAbstractButton::clicked, this, &WindowBar::maximizeRequested);
  return btn;
}

QAbstractButton* WindowBar::takeCloseButton() {
  Q_D(WindowBar);
  auto* btn = static_cast<QAbstractButton*>(d->takeWidgetAt(WindowBarPrivate::CloseButton));
  if (btn == nullptr) {
    return nullptr;
  }
  disconnect(btn, &QAbstractButton::clicked, this, &WindowBar::closeRequested);
  return btn;
}

QWidget* WindowBar::hostWidget() const {
  Q_D(const WindowBar);
  return d->w;
}

void WindowBar::setHostWidget(QWidget* w) {
  Q_D(WindowBar);

  QWidget* org = d->w;
  if (org != nullptr) {
    org->removeEventFilter(this);
  }
  d_ptr->w = w;
  if (w != nullptr) {
    w->installEventFilter(this);
  }
}

auto WindowBar::titleFollowWindow() const -> bool {
  Q_D(const WindowBar);
  return d->autoTitle;
}

void WindowBar::setTitleFollowWindow(bool value) {
  Q_D(WindowBar);
  d->autoTitle = value;
}

auto WindowBar::iconFollowWindow() const -> bool {
  Q_D(const WindowBar);
  return d->autoIcon;
}

void WindowBar::setIconFollowWindow(bool value) {
  Q_D(WindowBar);
  d->autoIcon = value;
}

auto WindowBar::eventFilter(QObject* obj, QEvent* event) -> bool {
  Q_D(WindowBar);
  auto* w = d->w;
  if (obj == w) {
    QAbstractButton* iconBtn = iconButton();
    QLabel* label = titleLabel();
    QAbstractButton* maxBtn = maxButton();
    switch (event->type()) {
      case QEvent::WindowIconChange: {
        if (d_ptr->autoIcon && (iconBtn != nullptr)) {
          iconBtn->setIcon(w->windowIcon());
          iconChanged(w->windowIcon());
        }
        break;
      }
      case QEvent::WindowTitleChange: {
        if (d_ptr->autoTitle && (label != nullptr)) {
          label->setText(w->windowTitle());
          titleChanged(w->windowTitle());
        }
        break;
      }
      case QEvent::WindowStateChange: {
        if (maxBtn != nullptr) {
          maxBtn->setChecked(w->isMaximized());
        }
        break;
      }
      default:
        break;
    }
  }
  return QWidget::eventFilter(obj, event);
}

void WindowBar::titleChanged(const QString& text) {
  Q_UNUSED(text)
}

void WindowBar::iconChanged(const QIcon& icon) {
  Q_UNUSED(icon)
}

WindowBar::WindowBar(WindowBarPrivate& d, QWidget* parent) : QFrame(parent), d_ptr(&d) {
  d.q_ptr = this;

  d.init();
}

}
