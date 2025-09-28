cmake_minimum_required(VERSION 3.16)

project(Glade VERSION 0.0.1 LANGUAGES CXX)

set(PROJECT_SOURCES
  src/main.cxx
  src/MainWindow.cxx
  src/DisplayWidget.cxx
  src/ActionController.cxx

  src/widget-frame/WindowBar.cxx
  src/widget-frame/WindowButton.cxx

  src/panels/AssetsPanel.cxx

  src/models/AssetTreeModel.cxx

  src/adapters/AssetAdapter.cxx

  resources/mainwindow.qrc

  src/dialogs/ModelDialog.ui
  src/dialogs/ModelDialog.cxx

  src/MainWindow.ui
)

set(QT_DEPS
  platforms/qwindows.dll
  platforms/qwindowsd.dll
  imageformats/qsvg.dll
  imageformats/qsvgd.dll
)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Svg)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Svg)

find_package(QWindowKit REQUIRED COMPONENTS Widgets Core)

qt_add_executable(${PROJECT_NAME}
  MANUAL_FINALIZATION
  ${PROJECT_SOURCES}
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
  Qt${QT_VERSION_MAJOR}::Widgets
  Qt${QT_VERSION_MAJOR}::Svg
  QWindowKit::Widgets
  base-kit
  engine::base
  engine::common
)

target_compile_definitions(${PROJECT_NAME}
  PRIVATE
  GLM_FORCE_RADIANS
  GLM_FORCE_DEPTH_ZERO_TO_ONE
  GLM_ENABLE_EXPERIMENTAL
  TRACY_NO_EXIT
  TRACY_CALLSTACK=5
  NOMINMAX
  DEBUG
)

target_precompile_headers(${PROJECT_NAME}
  PRIVATE

  <QAbstractButton>
  <QAbstractItemModel>
  <QActionGroup>
  <QApplication>
  <QBoxLayout>
  <QDialog>
  <QDockWidget>
  <QEvent>
  <QFile>
  <QFileDialog>
  <QDirIterator>
  <QFrame>
  <QIcon>
  <QLabel>
  <QMainWindow>
  <QMenu>
  <QMenuBar>
  <QMessageBox>
  <QSettings>
  <QSplitter>
  <QStatusBar>
  <QStyle>
  <QStyleFactory>
  <QTextEdit>
  <QTimer>
  <QToolBar>
  <QTreeView>
  <QtCore/QDebug>
  <QtCore/QLocale>
  <QtGui/QtEvents>
  <QtWidgets/QPushButton>
  <QUndoCommand>
  <QUndoStack>
  <QKeySequence>
  <qcoreapplication.h>
  <qnamespace.h>

  <any>
  <filesystem>
  <fstream>
  <functional>
  <future>
  <mutex>
  <optional>
  <queue>
  <ranges>
  <sstream>
  <string>
  <utility>
  <variant>
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
  .
  src
)

set(QT_PLUGINS_DIR "C:/Qt/6.9.1/msvc2022_64/plugins/")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  # Copy Qt runtime deps
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  $<TARGET_FILE:Qt6::Core>
  $<TARGET_FILE:Qt6::Gui>
  $<TARGET_FILE:Qt6::Widgets>
  $<TARGET_FILE:Qt6::Svg>
  $<TARGET_FILE:QWindowKit::Widgets>
  $<TARGET_FILE:QWindowKit::Core>
  $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

# Copy More Qt Runtime Deps
foreach(dep ${QT_DEPS})
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${QT_PLUGINS_DIR}/${dep}"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${dep}"
  )
endforeach()

qt_finalize_executable(${PROJECT_NAME})

add_dependencies(${PROJECT_NAME} assets)
