#ifndef EXPORT_HANDLER_H
#define EXPORT_HANDLER_H

#include <PinTableWidget.h>

#include <QObject>

namespace pinhelper {
/// @breif Class. This class is used to export the pin assignments to a file.
class ExportHandler : QObject {
  Q_OBJECT
 public:
  /// @brief Constructor.
  /// @param parent Parent widget.
  ExportHandler(QObject* parent = nullptr);

  enum class FileType { XML, JSON };

  /// @brief Destructor.
  ~ExportHandler();

  /// @brief Set the pin table widget.
  /// @param pin_table_widget Pin table widget.
  void setPinTableWidget(PinTableWidget* pin_table_widget);

  /// @brief Export the pin assignments to a file.
  /// @param filepath Path of the file.
  /// @param file_type Type of the file.
  void exportFile(const QString& filepath, FileType file_type = FileType::XML);

  /// @brief Export the pin assignments to a file.
  /// @param filepath Path of the file.
  /// @param file_type Type of the file.
  void exportFile(const QString& filepath, const QString& file_type);

 private:
  void exportXML(const QString& filepath);
  void exportJSON(const QString& filepath);

 private:
  PinTableWidget* pin_table_widget_;
};

}  // namespace pinhelper

#endif  // EXPORT_HANDLER_H
