#pragma once

#include "WindowBar.hpp"

namespace QWK {

class WindowBarPrivate {
  Q_DECLARE_PUBLIC(WindowBar)
public:
  WindowBarPrivate();
  virtual ~WindowBarPrivate();

  void init();

  WindowBar* q_ptr;

  QWidget* w;
  bool autoTitle;
  bool autoIcon;

  enum WindowBarItem : uint8_t {
    IconButton,
    MenuWidget,
    TitleLabel,
    MinimizeButton,
    MaximizeButton,
    CloseButton,
  };

  QHBoxLayout* layout;

  [[nodiscard]] auto widgetAt(int index) const -> QWidget* {
    return layout->itemAt(index)->widget();
  }

  void setWidgetAt(int index, QWidget* widget) const;

  [[nodiscard]] auto takeWidgetAt(int index) const -> QWidget*;

  void insertDefaultSpace(int index) const {
    layout->insertSpacerItem(index, new QSpacerItem(0, 0));
  }

private:
  Q_DISABLE_COPY(WindowBarPrivate)
};

}
