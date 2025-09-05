#include "DisplayWidget.hpp"

namespace gld {

void DisplayWidget::resizeEvent(QResizeEvent* event) {
  emit resized(event->size());
  QWidget::resizeEvent(event);
}

}
