#ifndef PIN_HELPER_H
#define PIN_HELPER_H

#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>

#include "FlatUI.h"
#include "PinTableWidget.h"

namespace pinhelper {

/// @brief Class for the main window of the application.
class PinHelper : public QMainWindow {
  Q_OBJECT

 public:
  /// @brief Constructor.
  /// @param parent Parent widget.
  PinHelper(QWidget* parent = nullptr);

  /// @brief Destructor.
  ~PinHelper() override;

 protected:
  /// @brief Size Hint
  QSize sizeHint() const override;

 private:
  /// @brief Initialize the UI.
  void initUi();

  /// @brief Initialize the connections.
  void initConnections();

 private slots:
  /// @brief Slot. Open file dialog when import button is clicked.
  void on_import_btn_clicked();

 private:
  PinTableWidget* pin_table_;
  
  QLabel* filepath_label_;
  QLineEdit* filepath_line_edit_;

  QPushButton* import_btn_;
  QPushButton* export_btn_;

  QLabel* export_type_label_;
  QComboBox* export_type_combo_box_;
};

}  // namespace pinhelper

#endif  // PIN_HELPER_H
