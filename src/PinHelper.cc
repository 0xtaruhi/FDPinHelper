#include "PinHelper.h"

#include <QFileDialog>
#include <QGroupBox>
#include <QLayout>

#include "FlatUI.h"


constexpr auto kDefaultWidth = 500;
constexpr auto kDefaultHeight = 300;
constexpr auto kMinimumWidth = 400;
constexpr auto kMinimumHeight = 300;

using namespace pinhelper;

PinHelper::PinHelper(QWidget *parent) : QMainWindow(parent) {
  initUi();
  initConnections();
}

PinHelper::~PinHelper() = default;

QSize PinHelper::sizeHint() const { return {kDefaultWidth, kDefaultHeight}; }

void PinHelper::initUi() {
//   this->setStyleSheet("background-color: white;");
  QWidget *central_widget = new QWidget(this);

  // Set Main Layout
  auto *main_layout = new QVBoxLayout(this);

  // Set Settings Group
  auto *settings_group = new QGroupBox(tr("Settings"), this);
  auto *settings_layout = new QHBoxLayout(this);

  filepath_label_ = new QLabel(tr("&File Path"), this);
  settings_layout->addWidget(filepath_label_);

  filepath_line_edit_ = new QLineEdit(this);
  filepath_line_edit_->setReadOnly(true);
  FlatUI::setLineEditQss(filepath_line_edit_);

  filepath_label_->setBuddy(filepath_line_edit_);
  
  settings_layout->addWidget(filepath_line_edit_);
  settings_group->setFixedHeight(filepath_line_edit_->height() + 40);

  import_btn_ = new QPushButton(tr("&Import"), this);
  FlatUI::setPushButtonQss(import_btn_);
  settings_layout->addWidget(import_btn_);
  settings_group->setLayout(settings_layout);

  main_layout->addWidget(settings_group);

  // Set Pin Table
  pin_table_ = new PinTableWidget(this);
  main_layout->addWidget(pin_table_);

  // Set Export Group
  auto *export_layout = new QHBoxLayout(this);
  export_layout->addStretch();

  export_type_label_ = new QLabel(tr("Export &Type"), this);
  export_type_combo_box_ = new QComboBox(this);
  export_type_combo_box_->addItems(QStringList({
    tr("XML"),
    tr("JSON")
  }));
  export_btn_ = new QPushButton(tr("&Export"), this);
  FlatUI::setPushButtonQss(export_btn_);

  export_type_label_->setBuddy(export_type_combo_box_);

  export_layout->addWidget(export_type_label_);
  export_layout->addWidget(export_type_combo_box_);
  export_layout->addWidget(export_btn_);
  main_layout->addLayout(export_layout);

  central_widget->setLayout(main_layout);
  central_widget->setMinimumHeight(kMinimumHeight);
  central_widget->setMinimumWidth(kMinimumWidth);

  this->setCentralWidget(central_widget);
}

void PinHelper::initConnections() {
  connect(import_btn_, &QPushButton::clicked, this, &on_import_btn_clicked);
}

void PinHelper::on_import_btn_clicked() {
  auto file_dialog = new QFileDialog();
  //   file_dialog->setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);

  QStringList name_filters;
  name_filters.append(tr("Verilog File (*.v)"));
  name_filters.append(tr("SystemVerilog File (*.sv)"));
  name_filters.append(tr("Any (*)"));
  file_dialog->setNameFilters(name_filters);

  file_dialog->setWindowTitle(tr("Import File"));
  file_dialog->setFileMode(QFileDialog::FileMode::ExistingFile);

  auto dialogClosed = [=](int code) {
    Q_UNUSED(code);
    delete file_dialog;
  };

  auto fileSelected = [=](const QString &filepath) {
    this->filepath_line_edit_->setText(filepath);
  };

  connect(file_dialog, &QFileDialog::finished, dialogClosed);
  connect(file_dialog, &QFileDialog::fileSelected, fileSelected);

  file_dialog->show();
}
