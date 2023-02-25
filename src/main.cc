#include <QApplication>

#include "PinHelper.h"

int main(int argc, char** argv) {
  QApplication app(argc, argv);
  app.setStyle("Fusion");
  pinhelper::PinHelper mw;
  mw.show();
  
  return app.exec();
}
