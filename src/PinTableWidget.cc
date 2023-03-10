#include "PinTableWidget.h"

#include <QHeaderView>
#include <QList>

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
  QList<QStandardItem *> row;
  row.resize(3);
  row[0] = new QStandardItem(pin.name());
  row[1] = new QStandardItem(getDirectionName(pin.direction()));
  row[2] = new QStandardItem(pin.pin());
  return row;
}

PinTableWidget::PinTableWidget(QWidget *parent) : QWidget(parent) {
  initUi();
  initConnections();
}

PinTableWidget::~PinTableWidget() = default;

void PinTableWidget::addItem(const PinItem &item) {
  pin_items_.push_back(item);
  table_model_->appendRow(getRow(item));
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

  table_view_->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void PinTableWidget::updateItem(const qsizetype index, const PinItem &item) {
  Q_ASSERT(index < pin_items_.size());
  auto &pin = pin_items_[index];
  pin.setName(item.name());
  pin.setDirection(item.direction());
  pin.setPin(item.pin());

  auto row = getRow(pin);
  for (int i = 0; i < row.size(); ++i) {
    table_model_->setItem(index, i, row[i]);
  }
}
