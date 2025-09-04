#include "MainWindow.hpp"
#include <qcoreapplication.h>

auto main(int argc, char* argv[]) -> int {
  QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
      Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
  QCoreApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
  QApplication::setHighDpiScaleFactorRoundingPolicy(
      Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

  QCoreApplication::setOrganizationDomain("Triton");
  QCoreApplication::setApplicationName("Glade");

  QApplication a(argc, argv);
  QApplication::setStyle("Fusion");
  QApplication::setWindowIcon(QIcon(":/app/gladev1.png"));

  MainWindow w;
  w.show();

  return QApplication::exec();
}
