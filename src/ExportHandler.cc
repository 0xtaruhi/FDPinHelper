#include "ExportHandler.h"

#include <QFile>
#include <QXmlStreamWriter>

using namespace pinhelper;

ExportHandler::ExportHandler(QObject* parent) : QObject(parent) {}

ExportHandler::~ExportHandler() {}

void ExportHandler::setPinTableWidget(PinTableWidget* pin_table_widget) {
  pin_table_widget_ = pin_table_widget;
}

void ExportHandler::exportFile(const QString& filepath, FileType file_type) {
  if (file_type == FileType::XML) {
    exportXML(filepath);
  } else if (file_type == FileType::JSON) {
    exportJSON(filepath);
  }
}

void ExportHandler::exportFile(const QString& filepath,
                               const QString& file_type) {
  if (file_type == "XML") {
    exportXML(filepath);
  } else if (file_type == "JSON") {
    exportJSON(filepath);
  }
}

void ExportHandler::exportXML(const QString& filepath) {
  QXmlStreamWriter writer;
  QFile file(filepath);

  if (!file.open(QIODevice::WriteOnly)) {
    throw QString(tr("Could not open file %1 for writing.").arg(filepath));
    return;
  }

  const auto& pin_assignments = pin_table_widget_->getItems();

  writer.setDevice(&file);
  writer.setAutoFormatting(true);
  writer.writeStartDocument();
  writer.writeStartElement("design");
  writer.writeAttribute("name", "design_name");

  for (const auto& p : pin_assignments) {
    writer.writeEmptyElement("port");
    writer.writeAttribute("name", p.name());
    writer.writeAttribute("position", p.pin());
  }

  writer.writeEndElement();
  writer.writeEndDocument();
}

void ExportHandler::exportJSON(const QString& filepath) {}
