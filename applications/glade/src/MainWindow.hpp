#pragma once

#include "actions/ActionManager.hpp"
#include "IEventQueue.hpp"
#include "IEngineContext.hpp"

namespace gld {
class AssetAdapter;
}

namespace QWK {
class WidgetWindowAgent;
class StyleAgent;
}

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow() override;

  enum class Theme : uint8_t {
    Dark,
    Light,
  };
  Q_ENUM(Theme)

Q_SIGNALS:
  void themeChanged();

protected:
  void closeEvent(QCloseEvent* event) override;
  void showEvent(QShowEvent* event) override;
  auto event(QEvent* event) -> bool override;

private:
  Ui::MainWindow* ui;

  auto setupActions() -> void;
  auto installWindowAgent() -> void;
  auto loadStyleSheet(Theme theme) -> void;

  QUndoStack* undoStack;

  Theme currentTheme{};

  QWK::WidgetWindowAgent* windowAgent;

  std::shared_ptr<arb::IEngineContext> context;
  std::shared_ptr<arb::IEventQueue> eventQueue;
  gld::AssetAdapter* assetAdapter;

  std::unique_ptr<gld::ActionManager> actionManager;
};
