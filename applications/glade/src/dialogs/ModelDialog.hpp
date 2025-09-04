#pragma once

#include "../ImportModelDetails.hpp"
#include <qevent.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class ModelDialog;
}
QT_END_NAMESPACE

namespace gld {

class ModelDialog : public QDialog {
  Q_OBJECT
public:
  explicit ModelDialog(QWidget* parent = nullptr);

  [[nodiscard]] auto details() const -> ImportModelDetails;

  void showEvent(QShowEvent* event) override;

signals:
  void acceptedWithDetails(const ImportModelDetails& details);

private slots:
  void onBrowseClicked();

private:
  Ui::ModelDialog* ui;
};

}
