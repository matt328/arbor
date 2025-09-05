#pragma once

namespace gld {

class DisplayWidget : public QWidget {
  Q_OBJECT
public:
  using QWidget::QWidget;

signals:
  void resized(QSize newSize);

protected:
  void resizeEvent(QResizeEvent* event) override;
};

}
