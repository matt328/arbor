#include "QtVulkanWidget.hpp"

namespace gld {

QtVulkanWidget::QtVulkanWidget(QWidget* parent) : QWidget{parent} {
  connect(this, &QtVulkanWidget::resized, this, [this](int w, int h, qreal dpr) {
    auto ramebufferWidth = w * dpr;
    auto framebufferHeight = h * dpr;
  });
}

void QtVulkanWidget::resizeEvent(QResizeEvent* event) {
  emit resized(width(), height(), devicePixelRatioF());
  QWidget::resizeEvent(event);
}

auto QtVulkanWidget::getHandle() const -> WId {
  return winId();
}

}
