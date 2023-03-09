#include "ModulesSelectDialog.h"

#include <QLayout>

using namespace pinhelper;

ModulesSelectDialog::ModulesSelectDialog(QWidget *parent)
    : QDialog(parent), items_() {
  initUi();
  initConnections();
}

ModulesSelectDialog::ModulesSelectDialog(const QStringList &items,
                                         QWidget *parent)
    : QDialog(parent), items_(items) {
  initUi();
  initConnections();
}

ModulesSelectDialog::~ModulesSelectDialog() = default;

void ModulesSelectDialog::addItems(const QStringList &items) {
  items_ << items;
}

void ModulesSelectDialog::addItem(const QString &item) {
  items_.push_back(item);
}

void ModulesSelectDialog::initUi() {
  hint_label_ = new QLabel(tr("Select a module:"), this);

  list_widget_ = new QListWidget(this);
  list_widget_->addItems(items_);
  ok_btn_ = new QPushButton(tr("OK"), this);
  cancel_btn_ = new QPushButton(tr("Cancel"), this);

  auto *layout = new QVBoxLayout(this);
  auto btn_layout = new QHBoxLayout();

  layout->addWidget(hint_label_);
  layout->addWidget(list_widget_);
  btn_layout->addStretch();
  btn_layout->addWidget(ok_btn_);
  btn_layout->addWidget(cancel_btn_);
  layout->addLayout(btn_layout);

  this->setLayout(layout);
  this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

void ModulesSelectDialog::initConnections() {
  connect(list_widget_, &QListWidget::itemDoubleClicked, this,
          &ModulesSelectDialog::on_ok_btn_clicked);
  connect(ok_btn_, &QPushButton::clicked, this,
          &ModulesSelectDialog::on_ok_btn_clicked);
  connect(cancel_btn_, &QPushButton::clicked, this,
          &ModulesSelectDialog::on_cancel_btn_clicked);
}

void ModulesSelectDialog::updateListWidget() {
  list_widget_->clear();
  list_widget_->addItems(items_);
}

void ModulesSelectDialog::clear() { items_.clear(); }

void ModulesSelectDialog::accept() {
  if (list_widget_->currentItem()) {
    emit itemSelected(list_widget_->currentItem()->text());
  }
  QDialog::accept();
  QDialog::close();
}

void ModulesSelectDialog::reject() {
  QDialog::reject();
  QDialog::close();
}

void ModulesSelectDialog::on_ok_btn_clicked() { accept(); }

void ModulesSelectDialog::on_cancel_btn_clicked() { reject(); }
