#ifndef PIN_TABLE_WIDGET_H
#define PIN_TABLE_WIDGET_H

#include <QList>
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

    PinItem() = default;
    PinItem(const QString &name, Direction direction, const QString &pin)
        : name_(name), direction_(direction), pin_(pin) {}

    auto name() const { return name_; }
    auto direction() const { return direction_; }
    auto pin() const { return pin_; }

    auto setName(const QString &name) { name_ = name; }
    auto setDirection(Direction direction) { direction_ = direction; }
    auto setPin(const QString &pin) { pin_ = pin; }

   private:
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

  /// @brief Method, Add Items to the table.
  void addItems(const QList<PinItem> &items);

  /// @brief Method, Get the pin items.
  const QList<PinItem> &getItems() const;

  /// @brief Method. Clear the table.
  void clear();

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
  void updateItem(const qsizetype index, const PinItem &item);

  /// @brief Update All Items.
  void updateItems();

 private:
  QTableView *table_view_;
  QStandardItemModel *table_model_;
  QList<PinItem> pin_items_;
};

}  // namespace pinhelper

#endif  // PIN_TABLE_WIDGET_H
