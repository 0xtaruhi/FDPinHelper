#include "DevicePinReader.h"

#include <QXmlStreamReader>

using namespace pinhelper;

DevicePinReader::DevicePinReader(QObject* parent) : QObject(parent) {}

DevicePinReader::~DevicePinReader() = default;

bool DevicePinReader::readDeviceInformation(const QString& filepath) {
  QFile file(filepath);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return false;
  }

  QXmlStreamReader xmlReader(&file);
  while (!xmlReader.atEnd() && !xmlReader.hasError()) {
    QXmlStreamReader::TokenType token = xmlReader.readNext();
    if (token == QXmlStreamReader::StartDocument) {
      continue;
    }
    if (token == QXmlStreamReader::StartElement) {
      if (xmlReader.name() == QString::fromStdString("device")) {
        continue;
      } else if (xmlReader.name() == QString::fromStdString("input")) {
        input_pins_.append(xmlReader.attributes().value("pin").toString());
      } else if (xmlReader.name() == QString::fromStdString("output")) {
        output_pins_.append(xmlReader.attributes().value("pin").toString());
      } else if (xmlReader.name() == QString::fromStdString("clock")) {
        clock_pin_ = xmlReader.attributes().value("pin").toString();
      } else {
        continue;
      }
    }
  }

  return true;
}

const QList<QString>& DevicePinReader::getInputPins() const {
  return input_pins_;
}

const QList<QString>& DevicePinReader::getOutputPins() const {
  return output_pins_;
}

const QString& DevicePinReader::getClockPin() const { return clock_pin_; }
