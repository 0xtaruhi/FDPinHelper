#include "PinTableWidget.h"

#include <QHeaderView>
#include <QVector>

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

PinTableWidget::PinTableWidget(QWidget *parent) : QWidget(parent) {
  initUi();
  initConnections();
}

PinTableWidget::~PinTableWidget() = default;

void PinTableWidget::addItem(const PinItem &item) {
  pin_items_.push_back(item);
}

void PinTableWidget::resizeEvent(QResizeEvent *event) {
  table_view_->setGeometry(0, 0, width(), height());
}

void PinTableWidget::initUi() {
  table_view_ = new QTableView(this);
  table_model_ = new QStandardItemModel(this);

  initTableModel();

  table_view_->setModel(table_model_);
}

void PinTableWidget::initConnections() {}

void PinTableWidget::initTableModel() {
  table_model_->setColumnCount(4);
  table_model_->setRowCount(3);

  const QVector<QString> header_name = {tr("Index"), tr("Name"),
                                        tr("Direction"), tr("Pin")};
  for (int i = 0; i < header_name.size(); ++i) {
    table_model_->setHeaderData(i, Qt::Horizontal, header_name[i]);
  }
}

void PinTableWidget::initTableView() {
  auto *header = table_view_->horizontalHeader();
  header->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void PinTableWidget::updateItem(const size_t index, const PinItem &item) {
  QVector<QStandardItem *> row_items{
      new QStandardItem(QString::number(item.index())),
      new QStandardItem(item.name()),
      new QStandardItem(getDirectionName(item.direction())),
      new QStandardItem(item.pin())};

  for (int i = 0; i < row_items.size(); ++i) {
    table_model_->setItem(index, i, row_items[i]);
  }
}
