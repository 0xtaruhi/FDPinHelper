#include "PinTableWidget.h"

#include <QComboBox>
#include <QHeaderView>
#include <QList>

// #include "ComboBoxItemDelegate.h"

using namespace pinhelper;

auto getDirectionName(const PinTableWidget::PinItem::Direction &direction) {
  switch (direction) {
    case PinTableWidget::PinItem::Direction::Input:
      return "Input";
    case PinTableWidget::PinItem::Direction::Output:
      return "Output";
    case PinTableWidget::PinItem::Direction::Inout:
      return "Inout";
    case PinTableWidget::PinItem::Direction::Unknown:
      return "Unknown";
  }
  return "";
}

auto getRow(const PinTableWidget::PinItem &pin) {
  QList<QStandardItem *> row({
      new QStandardItem(pin.name()),
      new QStandardItem(getDirectionName(pin.direction()))
      // new QStandardItem(pin.pin())
  });
  return row;
}

PinTableWidget::PinTableWidget(QWidget *parent) : QWidget(parent) {
  initUi();
  initConnections();
}

PinTableWidget::~PinTableWidget() = default;

void PinTableWidget::setDevicePinReader(DevicePinReader *device_pin_reader) {
  device_pin_reader_ = device_pin_reader;
}

void PinTableWidget::addItem(const PinItem &item) {
  pin_items_.push_back(item);
  table_model_->appendRow(getRow(item));
  table_view_->setIndexWidget(table_model_->index(pin_items_.size() - 1, 2),
                              createComboBox(item, item.pin()));
}

void PinTableWidget::addItems(const QList<PinItem> &items) {
  for (const auto &item : items) {
    addItem(item);
  }
}

const QList<PinTableWidget::PinItem> &PinTableWidget::getItems() const {
  return pin_items_;
}

void PinTableWidget::clear() {
  pin_items_.clear();
  table_model_->clear();
  initTableModel();
}

void PinTableWidget::resizeEvent(QResizeEvent *event) {
  table_view_->setGeometry(0, 0, width(), height());
}

void PinTableWidget::initUi() {
  table_view_ = new QTableView(this);
  table_model_ = new QStandardItemModel(this);

  initTableModel();
  initTableView();

  table_view_->setModel(table_model_);
}

void PinTableWidget::initConnections() {}

void PinTableWidget::initTableModel() {
  table_model_->setColumnCount(3);

  const QVector<QString> header_name = {tr("Name"), tr("Direction"), tr("Pin")};
  for (int i = 0; i < header_name.size(); ++i) {
    table_model_->setHeaderData(i, Qt::Horizontal, header_name[i]);
  }
}

void PinTableWidget::initTableView() {
  auto *header = table_view_->horizontalHeader();
  // header->setSectionResizeMode(QHeaderView::ResizeToContents);
  header->setMinimumWidth(80);
  header->setStretchLastSection(true);

  // combo_box_item_delegate_ = new ComboBoxItemDelegate(this);
  // table_view_->setItemDelegateForColumn(2, combo_box_item_delegate_);
  // table_view_->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void PinTableWidget::updateItem(const qsizetype index, const PinItem &item) {
  Q_ASSERT(index < pin_items_.size());
  auto &pin = pin_items_[index];
  pin.setName(item.name());
  pin.setDirection(item.direction());
  pin.setPin(item.pin());

  auto row = getRow(pin);
  for (qsizetype i = 0; i != row.size(); ++i) {
    table_model_->setItem(index, static_cast<int>(i), row[i]);
  }
}

QComboBox *PinTableWidget::createComboBox(const PinItem &pin,
                                          const QString &default_pin) {
  QComboBox *cb = new QComboBox(this);
  if (device_pin_reader_) {
    if (pin.isClock()) {
      cb->addItem(device_pin_reader_->getClockPin());
    } else {
      switch (pin.direction()) {
        case PinItem::Direction::Input:
          cb->addItems(device_pin_reader_->getInputPins());
          break;
        case PinItem::Direction::Output:
          cb->addItems(device_pin_reader_->getOutputPins());
          break;
        default:
          break;
      };
    }
    cb->setCurrentText(default_pin);
  }
  return cb;
}
