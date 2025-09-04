#pragma once

QT_BEGIN_NAMESPACE
namespace Ui {
class LogPanel;
}
QT_END_NAMESPACE

namespace gld {

class LogPanel : public QWidget {
  Q_OBJECT
public:
  explicit LogPanel(QWidget* parent = nullptr);

private:
  Ui::LogPanel* ui;

  auto eventFilter(QObject* obj, QEvent* event) -> bool override;
};

}
