#include <QApplication>
#include <QTranslator>

#include "PinHelper.h"

int main(int argc, char** argv) {
  QApplication app(argc, argv);
  QTranslator* tr = new QTranslator;

  bool tr_loaded =
      tr->load(QString("FDPinHelper_%1").arg(QLocale::system().name()),
               ":/translations");

  Q_UNUSED(tr_loaded);

  app.installTranslator(tr);
  app.setStyle("Fusion");
  pinhelper::PinHelper mw;
  mw.show();

  return app.exec();
}
