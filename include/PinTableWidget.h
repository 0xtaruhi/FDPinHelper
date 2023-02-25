#ifndef PIN_TABLE_WIDGET_H
#define PIN_TABLE_WIDGET_H

#include <QStandardItemModel>
#include <QString>
#include <QTableView>
#include <QWidget>

namespace pinhelper {

/// @brief Class for the table widget.
class PinTableWidget : public QWidget {
  Q_OBJECT

 public:
  /// @brief Class for the pin item.
  class PinItem {
   public:
    /// @brief Enum for the port direction.
    enum class Direction { Input, Output, Inout, Unknown };

    PinItem(int index, const QString &name, Direction direction,
            const QString &pin)
        : index_(index), name_(name), direction_(direction), pin_(pin) {}

    auto index() const { return index_; }
    auto name() const { return name_; }
    auto direction() const { return direction_; }
    auto pin() const { return pin_; }

   private:
    int index_;
    QString name_;
    Direction direction_;
    QString pin_;
  };

 public:
  /// @brief Constructor.
  /// @param parent Parent widget.
  PinTableWidget(QWidget *parent = nullptr);

  /// @brief Destructor.
  ~PinTableWidget() override;

  /// @brief Method. Add Item to the table.
  void addItem(const PinItem &item);

 protected:
  /// @brief Override the resize event.
  /// @param event Event.
  void resizeEvent(QResizeEvent *event) override;

 private:
  /// @brief Initialize the UI.
  void initUi();

  /// @brief Initialize the connections.
  void initConnections();

  /// @brief Inititalize the table model.
  void initTableModel();

  /// @brief Initialize the table view.
  void initTableView();

  /// @brief Update Specific Item.
  /// @param item Item.
  void updateItem(const size_t index, const PinItem &item);

  /// @brief Update All Items.
  void updateItems();

 private:
  QTableView *table_view_;
  QStandardItemModel *table_model_;
  QVector<PinItem> pin_items_;
};

}  // namespace pinhelper

#endif  // PIN_TABLE_WIDGET_H
