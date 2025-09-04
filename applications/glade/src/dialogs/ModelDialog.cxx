#include "ModelDialog.hpp"

#include "./ui_ModelDialog.h"

namespace gld {

ModelDialog::ModelDialog(QWidget* parent) : QDialog(parent), ui(new Ui::ModelDialog) {
  ui->setupUi(this);

  connect(ui->toolButton, &QPushButton::clicked, this, &ModelDialog::onBrowseClicked);
  connect(this, &QDialog::accepted, [this]() {
    auto d = ImportModelDetails{.filename = ui->lineEditFile->text(),
                                .identifier = ui->lineEdit->text()};
    emit acceptedWithDetails(d);
  });
}

auto ModelDialog::onBrowseClicked() -> void {
  auto settings = QSettings{};
  const auto* const key = "lastModelDir";
  auto startDir = settings.value(key, QDir::homePath()).toString();
  QString file = QFileDialog::getOpenFileName(this,
                                              tr("Select Model"),
                                              startDir,
                                              tr("Model Files (*.trm);;All Files (*.*)"));
  if (!file.isEmpty()) {
    ui->lineEditFile->setText(file);
    settings.setValue(key, QFileInfo(file).absolutePath());
  }
}

void ModelDialog::showEvent(QShowEvent* event) {
  ui->lineEdit->clear();
  ui->lineEditFile->clear();
  ui->lineEdit->setFocus(Qt::FocusReason::PopupFocusReason);
}

auto ModelDialog::details() const -> ImportModelDetails {
  ImportModelDetails d;
  d.filename = ui->lineEditFile->text();
  d.identifier = "default";
  return d;
}
}
