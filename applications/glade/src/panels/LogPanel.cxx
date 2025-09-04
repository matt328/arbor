#include "LogPanel.hpp"

#include "bk/Logger.hpp"
#include "spdlog/sinks/qt_sinks.h"

#include "./ui_LogPanel.h"
#include <qtextcursor.h>

namespace gld {

LogPanel::LogPanel(QWidget* parent) : QWidget{parent}, ui(new Ui::LogPanel) {
  ui->setupUi(this);
  auto logger = spdlog::qt_color_logger_mt("QLogger", ui->textEdit, 200);
  bk::LoggerManager::registerQtSink(logger->sinks().front());

  ui->textEdit->setStyleSheet(R"(
    QTextEdit {
        background-color: #292929;
        color: #b3b3b3;
        selection-background-color: #264f78;
        selection-color: #ffffff;
    }
)");

  connect(ui->textEdit, &QTextEdit::textChanged, this, [this]() {
    ui->textEdit->moveCursor(QTextCursor::End);
  });

  ui->textEdit->installEventFilter(this);
}

auto LogPanel::eventFilter(QObject* obj, QEvent* event) -> bool {
  if (obj == ui->textEdit && event->type() == QEvent::Resize) {
    ui->textEdit->moveCursor(QTextCursor::End);
    ui->textEdit->ensureCursorVisible();
  }
  return QWidget::eventFilter(obj, event);
}

}
