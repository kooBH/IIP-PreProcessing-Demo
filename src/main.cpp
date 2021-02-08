#include <Qt>
#include <QApplication>
#include "DemoControl.h"

int main(int argc, char* argv[]){ 
  QCoreApplication::addLibraryPath(".");
  QCoreApplication::addLibraryPath("../lib");

  QApplication app(argc, argv);

  DemoControl demo;
  demo.show();

  return app.exec();
}
