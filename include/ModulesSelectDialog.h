#ifndef MODULES_SELECT_DIALOG_H
#define MODULES_SELECT_DIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QStringList>
#include <QLabel>

namespace pinhelper {

/// @brief Class for selecting single module from a multi-modules verilog file.

class ModulesSelectDialog : public QDialog {
  Q_OBJECT
 public:
  /// @brief Constructor.
  /// @param parent Parent widget.
  ModulesSelectDialog(QWidget *parent = nullptr);
  ModulesSelectDialog(const QStringList &items, QWidget *parent = nullptr);

  /// @brief Destructor.
  ~ModulesSelectDialog() override;

  /// @brief Method. Add Items to the list.
  void addItems(const QStringList &items);

  /// @brief Method. Add Item to the list.
  void addItem(const QString &item);

  /// @brief Method. Update the list widget.
  void updateListWidget();

  /// @brief Method. Clear all items in the list.
  void clear();

 private:
  /// @brief Initialize the UI.
  void initUi();

  /// @brief Initialize the connections.
  void initConnections();

 private:
  /// @brief Method. Accept the dialog.
  void accept() override;

  /// @brief Method. Reject the dialog.
  void reject() override;

 public slots:
  /// @brief Slot. Accept the dialog when ok button is clicked.
  void on_ok_btn_clicked();

  /// @brief Slot. Reject the dialog when cancel button is clicked.
  void on_cancel_btn_clicked();

 signals:
  void itemSelected(const QString& module_name);

 private:
  QStringList items_;

  QLabel *hint_label_;
  QListWidget *list_widget_;
  QPushButton *ok_btn_;
  QPushButton *cancel_btn_;
};

}  // namespace pinhelper

#endif  // MODULES_SELECT_DIALOG_H
