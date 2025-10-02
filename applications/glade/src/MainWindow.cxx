#include "MainWindow.hpp"

#include <cpptrace/cpptrace.hpp>
#include <cpptrace/from_current.hpp>
#include <cpptrace/formatting.hpp>

#include "bk/Logger.hpp"
#include "commands/ImportModelCommand.hpp"
#include "engine/common/EngineOptions.hpp"
#include "ui_MainWindow.h"

#include "dialogs/ModelDialog.hpp"

#include <QWKWidgets/widgetwindowagent.h>

#include "panels/AssetsPanel.hpp"
#include "widget-frame/WindowBar.hpp"
#include "widget-frame/WindowButton.hpp"

#include "bk/NativeWindowHandle.hpp"
#include "engine/common/ResizeEvent.hpp"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ui{new Ui::MainWindow()},
      actionManager{std::make_unique<gld::ActionManager>(this)} {
  ui->setupUi(this);

  undoStack = new QUndoStack(this);

  InitLogger("Main");
  Log::info("Logger Initialized");

  setupActions();
  installWindowAgent();

  auto* assetsPanel = new gld::AssetsPanel(actionManager->assetActions, this);

  ui->tabWidget->addTab(new QTextEdit("Game Objects Here"), "Game Objects");
  ui->tabWidget->addTab(assetsPanel, "Assets");
  ui->tabWidget->addTab(new QTextEdit("Tool 3"), "Asset Tool");

  auto handle = bk::NativeWindowHandle{};
  handle.set<WId>(ui->displayWidget->winId());

  const auto options = arb::EngineOptions{
      .debugEnabled = true,
      .initialSurfaceState = {
          .swapchainExtent =
              arb::Size{.width = static_cast<uint32_t>(ui->displayWidget->width()),
                        .height = static_cast<uint32_t>(ui->displayWidget->height())},
          .renderScale = 1.f,
      }};

  context = arb::makeEngineContext(handle, options);

  auto updateTimer = new QTimer(this);
  connect(updateTimer, &QTimer::timeout, this, [this]() {
    try {
      context->update(); // rethrows exceptions from engine threads
    } catch (const cpptrace::exception& e) {
      auto formatter = cpptrace::get_default_formatter();
      formatter.symbols(cpptrace::formatter::symbol_mode::pruned);
      formatter.colors(cpptrace::formatter::color_mode::always);
      formatter.addresses(cpptrace::formatter::address_mode::none);
      formatter.paths(cpptrace::formatter::path_mode::basename);
      formatter.hide_exception_machinery(true);
      Log::error("Engine exception: {} {}", e.message(), formatter.format(e.trace()));
      QMessageBox::critical(this, "Engine Error", e.message());
      qApp->quit();
    }
  });
  updateTimer->start(16); // ~60Hz update, adjust as needed

  eventQueue = context->getEventQueue();

  assetAdapter = new gld::AssetAdapter(eventQueue, this);

  connect(ui->displayWidget, &gld::DisplayWidget::resized, this, [&](QSize size) {
    eventQueue->emitEvent<arb::ResizeEvent>(
        arb::ResizeEvent{.width = static_cast<uint32_t>(size.width()),
                         .height = static_cast<uint32_t>(size.height())});
  });
}

MainWindow::~MainWindow() {
  delete ui;
}

auto MainWindow::showEvent(QShowEvent* event) -> void {
  auto settings = QSettings{};
  restoreGeometry(settings.value("geometry").toByteArray());
  restoreState(settings.value("windowState").toByteArray());
  ui->tabWidget->setCurrentIndex(settings.value("tabWidgetIndex", 0).toInt());
  ui->horizontalSplitter->restoreState(settings.value("horizontalSplitter").toByteArray());
  ui->verticalSplitter->restoreState(settings.value("verticalSplitter").toByteArray());
}

auto MainWindow::setupActions() -> void {
  auto* dialog = new gld::ModelDialog(this);
  connect(actionManager->assetActions.importModel, &QAction::triggered, dialog, &QDialog::exec);
  connect(dialog,
          &gld::ModelDialog::acceptedWithDetails,
          this,
          [this](const gld::ImportModelDetails& details) {
            undoStack->push(new gld::ImportModelCommand(assetAdapter, details));
          });
}

void MainWindow::closeEvent(QCloseEvent* event) {
  auto settings = QSettings{};
  settings.setValue("geometry", saveGeometry());
  settings.setValue("windowState", saveState());
  settings.setValue("horizontalSplitter", ui->horizontalSplitter->saveState());
  settings.setValue("verticalSplitter", ui->verticalSplitter->saveState());
  settings.setValue("tabWidgetIndex", ui->tabWidget->currentIndex());
  QMainWindow::closeEvent(event);
}

auto MainWindow::event(QEvent* event) -> bool {
  switch (event->type()) {
    case QEvent::WindowActivate: {
      auto* menu = menuWidget();
      if (menu != nullptr) {
        menu->setProperty("bar-active", true);
        style()->polish(menu);
      }
      break;
    }
    case QEvent::WindowDeactivate: {
      auto* menu = menuWidget();
      if (menu != nullptr) {
        menu->setProperty("bar-active", false);
        style()->polish(menu);
      }
      break;
    }
    default:
      break;
  }
  return QMainWindow::event(event);
}

static inline void emulateLeaveEvent(QWidget* widget) {
  Q_ASSERT(widget);
  if (widget == nullptr) {
    return;
  }
  QTimer::singleShot(0, widget, [widget]() {
    const QScreen* screen = widget->screen();
    const QPoint globalPos = QCursor::pos(screen);
    if (!QRect(widget->mapToGlobal(QPoint{0, 0}), widget->size()).contains(globalPos)) {
      QCoreApplication::postEvent(widget, new QEvent(QEvent::Leave));
      if (widget->testAttribute(Qt::WA_Hover)) {
        const QPoint localPos = widget->mapFromGlobal(globalPos);
        const QPoint scenePos = widget->window()->mapFromGlobal(globalPos);
        static constexpr const auto oldPos = QPoint{};
        const Qt::KeyboardModifiers modifiers = QGuiApplication::keyboardModifiers();
        auto* const event =
            new QHoverEvent(QEvent::HoverLeave, scenePos, globalPos, oldPos, modifiers);
        Q_UNUSED(localPos);
        QCoreApplication::postEvent(widget, event);
      }
    }
  });
}

auto MainWindow::installWindowAgent() -> void {
  loadStyleSheet(Theme::Dark);
  currentTheme = Theme::Dark;
  windowAgent = new QWK::WidgetWindowAgent(this);
  windowAgent->setup(this);

  auto* menuBar = [this]() {
    auto* menuBar = new QMenuBar(this);

    auto* file = new QMenu(tr("File"), menuBar);
    file->addAction(new QAction(tr("New"), menuBar));
    file->addAction(new QAction(tr("Open"), menuBar));
    file->addSeparator();

    auto* edit = new QMenu(tr("Edit"), menuBar);

    auto* undoAction = undoStack->createUndoAction(menuBar, tr("&Undo"));
    undoAction->setIcon(QIcon(":/app/undo.svg"));
    undoAction->setShortcut(QKeySequence::Undo);

    auto* redoAction = undoStack->createRedoAction(menuBar, tr("&Redo"));
    redoAction->setIcon(QIcon(":/app/redo.svg"));
    redoAction->setShortcut(QKeySequence::Redo);

    edit->addAction(undoAction);
    edit->addAction(redoAction);

    auto* assets = new QMenu(tr("Assets"), menuBar);
    assets->addAction(actionManager->assetActions.importModel);
    assets->addAction(actionManager->assetActions.importAnimation);
    assets->addAction(actionManager->assetActions.importSkeleton);

    auto* darkAction = new QAction(tr("Enable dark theme"), menuBar);
    darkAction->setCheckable(true);
    darkAction->setChecked(currentTheme == Theme::Dark);
    connect(darkAction, &QAction::triggered, this, [this](bool checked) {
      loadStyleSheet(checked ? Theme::Dark : Theme::Light);
    });
    connect(this, &MainWindow::themeChanged, darkAction, [this, darkAction]() {
      darkAction->setChecked(currentTheme == Theme::Dark);
    });

    auto* noneAction = new QAction(tr("None"), menuBar);
    noneAction->setData(QStringLiteral("none"));
    noneAction->setCheckable(true);
    noneAction->setChecked(false);

    auto* dwmBlurAction = new QAction(tr("Enable DWM blur"), menuBar);
    dwmBlurAction->setData(QStringLiteral("dwm-blur"));
    dwmBlurAction->setCheckable(true);
    dwmBlurAction->setChecked(true);

    auto* acrylicAction = new QAction(tr("Enable acrylic material"), menuBar);
    acrylicAction->setData(QStringLiteral("acrylic-material"));
    acrylicAction->setCheckable(true);

    auto* micaAction = new QAction(tr("Enable mica"), menuBar);
    micaAction->setData(QStringLiteral("mica"));
    micaAction->setCheckable(true);

    auto* micaAltAction = new QAction(tr("Enable mica alt"), menuBar);
    micaAltAction->setData(QStringLiteral("mica-alt"));
    micaAltAction->setCheckable(true);

    auto* winStyleGroup = new QActionGroup(menuBar);
    winStyleGroup->addAction(noneAction);
    winStyleGroup->addAction(dwmBlurAction);
    winStyleGroup->addAction(acrylicAction);
    winStyleGroup->addAction(micaAction);
    winStyleGroup->addAction(micaAltAction);
    connect(winStyleGroup, &QActionGroup::triggered, this, [this, winStyleGroup](QAction* action) {
      // Unset all custom style attributes first, otherwise the style will not display
      // correctly
      for (const QAction* _act : winStyleGroup->actions()) {
        const QString data = _act->data().toString();
        if (data.isEmpty() || data == QStringLiteral("none")) {
          continue;
        }
        windowAgent->setWindowAttribute(data, false);
      }
      const QString data = action->data().toString();
      if (data == QStringLiteral("none")) {
        setProperty("custom-style", false);
      } else if (!data.isEmpty()) {
        windowAgent->setWindowAttribute(data, true);
        setProperty("custom-style", true);
      }
      style()->polish(this);
    });

    auto* windowMenu = new QMenu(tr("Window"), menuBar);
    windowMenu->addAction(darkAction);
    windowMenu->addSeparator();
    windowMenu->addAction(noneAction);
    windowMenu->addAction(dwmBlurAction);
    windowMenu->addAction(acrylicAction);
    windowMenu->addAction(micaAction);
    windowMenu->addAction(micaAltAction);

    menuBar->addMenu(file);
    menuBar->addMenu(edit);
    menuBar->addMenu(assets);
    menuBar->addMenu(windowMenu);
    return menuBar;
  }();

  menuBar->setObjectName(QStringLiteral("win-menu-bar"));

  auto* titleLabel = new QLabel();
  titleLabel->setAlignment(Qt::AlignCenter);
  titleLabel->setObjectName(QStringLiteral("win-title-label"));

  auto* iconButton = new QWK::WindowButton();
  iconButton->setObjectName(QStringLiteral("icon-button"));
  iconButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

  auto* minButton = new QWK::WindowButton();
  minButton->setObjectName(QStringLiteral("min-button"));
  minButton->setProperty("system-button", true);
  minButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

  auto* maxButton = new QWK::WindowButton();
  maxButton->setCheckable(true);
  maxButton->setObjectName(QStringLiteral("max-button"));
  maxButton->setProperty("system-button", true);
  maxButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

  auto* closeButton = new QWK::WindowButton();
  closeButton->setObjectName(QStringLiteral("close-button"));
  closeButton->setProperty("system-button", true);
  closeButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

  auto* windowBar = new QWK::WindowBar();

  windowBar->setIconButton(iconButton);
  windowBar->setMinButton(minButton);
  windowBar->setMaxButton(maxButton);
  windowBar->setCloseButton(closeButton);

  windowBar->setMenuBar(menuBar);
  windowBar->setTitleLabel(titleLabel);
  windowBar->setHostWidget(this);

  windowAgent->setTitleBar(windowBar);

  windowAgent->setSystemButton(QWK::WindowAgentBase::WindowIcon, iconButton);
  windowAgent->setSystemButton(QWK::WindowAgentBase::Minimize, minButton);
  windowAgent->setSystemButton(QWK::WindowAgentBase::Maximize, maxButton);
  windowAgent->setSystemButton(QWK::WindowAgentBase::Close, closeButton);

  windowAgent->setHitTestVisible(menuBar, true);

  setMenuWidget(windowBar);

  connect(windowBar, &QWK::WindowBar::minimizeRequested, this, &QWidget::showMinimized);
  connect(windowBar, &QWK::WindowBar::maximizeRequested, this, [this, maxButton](bool max) {
    if (max) {
      showMaximized();
    } else {
      showNormal();
    }

    emulateLeaveEvent(maxButton);
  });
  connect(windowBar, &QWK::WindowBar::closeRequested, this, &QWidget::close);

  windowAgent->setWindowAttribute(tr("dwm-blur"), true);
  setProperty("custom-style", true);
  style()->polish(this);
}

void MainWindow::loadStyleSheet(Theme theme) {
  if (!styleSheet().isEmpty() && theme == currentTheme) {
    return;
  }
  currentTheme = theme;

  const QString path = (theme == Theme::Dark) ? QStringLiteral(":/styles/dark-style.qss")
                                              : QStringLiteral(":/styles/light-style.qss");

  QFile qss(path);
  if (!qss.exists()) {
    qWarning().noquote() << "Stylesheet file not found:" << path;
    return;
  }

  if (!qss.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qWarning().noquote() << "Failed to open stylesheet:" << path << "error:" << qss.errorString();
    return;
  }

  const QByteArray data = qss.readAll();
  if (data.isEmpty()) {
    qWarning().noquote() << "Stylesheet is empty:" << path;
    return;
  }

  setStyleSheet(QString::fromUtf8(data));
  Q_EMIT themeChanged();
}
