#include "PinHelper.h"

#include <QFile>
#include <QFileDialog>
#include <QGroupBox>
#include <QLayout>
#include <QMessageBox>
#include <QResizeEvent>
#include <string>

#include "Config.h"
#include "FlatUI.h"
#include "ModulesSelectDialog.h"

constexpr auto kDefaultWidth = 500;
constexpr auto kDefaultHeight = 400;
constexpr auto kMinimumWidth = 400;
constexpr auto kMinimumHeight = 350;

using namespace pinhelper;

PinHelper::PinHelper(QWidget *parent) : QMainWindow(parent) {
  initUi();
  auto_assign_handler_ = new AutoAssignHandler(this);
  export_handler_ = new ExportHandler(this);
  export_handler_->setPinTableWidget(pin_table_);
  module_select_dialog_handler_ = new ModulesSelectDialog(this);
  initConnections();
}

PinHelper::~PinHelper() = default;

QSize PinHelper::sizeHint() const { return {kDefaultWidth, kDefaultHeight}; }

void PinHelper::initUi() {
  this->setWindowTitle(tr("Pin Helper").append(" - ").append(PROJECT_VERSION));

  //   this->setStyleSheet("background-color: white;");
  QWidget *central_widget = new QWidget(this);

  // Set Main Layout
  auto *main_layout = new QVBoxLayout(this);

  // Set Settings Group
  auto *settings_group = new QGroupBox(tr("Settings"), this);
  auto *settings_layout = new QVBoxLayout(this);
  auto *settings_file_layout = new QHBoxLayout(this);
  auto *settings_explicit_clock_layout = new QHBoxLayout(this);

  filepath_label_ = new QLabel(tr("&File Path"), this);
  settings_file_layout->addWidget(filepath_label_);

  // Create A QVBoxLayout which contains a QLineEdit and a Spacer
  // to make the QLineEdit stretch to the right.
  auto *filepath_line_edit_layout = new QVBoxLayout(this);
  auto filepath_line_edit_spacer =
      new QSpacerItem(0, 0, QSizePolicy::Expanding);
  filepath_line_edit_ = new QLineEdit(this);
  filepath_line_edit_->setReadOnly(true);
  FlatUI::setLineEditQss(filepath_line_edit_);
  filepath_line_edit_layout->addItem(filepath_line_edit_spacer);
  filepath_line_edit_layout->addWidget(filepath_line_edit_);

  filepath_label_->setBuddy(filepath_line_edit_);
  settings_file_layout->addLayout(filepath_line_edit_layout);

  import_btn_ = new QPushButton(tr("&Import"), this);
  FlatUI::setPushButtonQss(import_btn_);
  settings_file_layout->addWidget(import_btn_);
  settings_layout->addLayout(settings_file_layout);

  explicit_clock_checkbox_ = new QCheckBox(tr("&Automatic Clock Pin"), this);
  explicit_clock_label_ = new QLabel(tr("Clock Pin Name"), this);
  explicit_clock_line_edit_ = new QLineEdit(this);
  FlatUI::setLineEditQss(explicit_clock_line_edit_);
  explicit_clock_label_->setBuddy(explicit_clock_line_edit_);

  settings_explicit_clock_layout->addWidget(explicit_clock_checkbox_);
  settings_explicit_clock_layout->addWidget(explicit_clock_label_);
  settings_explicit_clock_layout->addWidget(explicit_clock_line_edit_);
  settings_layout->addLayout(settings_explicit_clock_layout);
  explicit_clock_checkbox_->setCheckState(Qt::CheckState::Checked);
  on_explicit_clock_checkbox_stateChanged(Qt::CheckState::Checked);

  settings_layout->setContentsMargins(10, 10, 10, 10);

  settings_group->setLayout(settings_layout);
  settings_group->setFixedHeight(settings_layout->sizeHint().height() + 30);

  main_layout->addWidget(settings_group);

  // Set Pin Table
  pin_table_ = new PinTableWidget(this);
  main_layout->addWidget(pin_table_);

  // Set Export Group
  auto *export_layout = new QHBoxLayout(this);
  export_layout->addStretch();

  export_type_label_ = new QLabel(tr("Export &Type"), this);
  export_type_combo_box_ = new QComboBox(this);
  export_type_combo_box_->addItems(QStringList({tr("XML"), tr("JSON")}));
  export_btn_ = new QPushButton(tr("&Export"), this);
  FlatUI::setPushButtonQss(export_btn_, 5, 8, "#3498DB", "#FFFFFF", "#5DACE4",
                           "#E5FEFF", "#2483C7", "#A0DAFB");

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
  connect(explicit_clock_checkbox_, &QCheckBox::stateChanged, this,
          &on_explicit_clock_checkbox_stateChanged);

  connect(export_btn_, &QPushButton::clicked, this, &on_export_btn_clicked);
  connect(auto_assign_handler_, &AutoAssignHandler::multipleModulesFound, this,
          &multiModulesProcess);
  connect(module_select_dialog_handler_, &ModulesSelectDialog::itemSelected,
          this, &on_module_selected);
}

void PinHelper::probeVerilogFile() {
  auto filepath = filepath_line_edit_->text();

  try {
    auto_assign_handler_->setFilepath(filepath);
    auto_assign_handler_->resolveModules();
  } catch (const QString &msg) {
    QMessageBox::warning(this, tr("Warning"), msg);
    return;
  }

  const auto &ports = auto_assign_handler_->getPorts();
  pin_table_->clear();

  auto explicit_clock = !explicit_clock_checkbox_->isChecked();
  auto clock_name = explicit_clock ? explicit_clock_line_edit_->text() : "clk";
  // auto pins = AutoAssignHandler::defaultAssign(ports, clock_name, "FDP3P7");
  QList<PinTableWidget::PinItem> pins;
  try {
    pins = auto_assign_handler_->defaultAssign(ports, clock_name, "FDP3P7");
  } catch (const QString &msg) {
    QMessageBox::warning(this, tr("Warning"), msg);
    return;
  }

  pin_table_->addItems(pins);
}

void PinHelper::multiModulesProcess(const QStringList &module_names) {
  module_select_dialog_handler_->clear();
  module_select_dialog_handler_->addItems(module_names);
  module_select_dialog_handler_->updateListWidget();

  module_select_dialog_handler_->exec();
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
    probeVerilogFile();
  };

  connect(file_dialog, &QFileDialog::finished, dialogClosed);
  connect(file_dialog, &QFileDialog::fileSelected, fileSelected);

  file_dialog->exec();
}

void PinHelper::on_export_btn_clicked() {
  auto write_filepath = QFileDialog::getSaveFileName(nullptr, tr("Export"), "",
                                                     tr("XML Files (*.xml)"));
  export_handler_->setModuleName(auto_assign_handler_->getModuleName());
  try {
    export_handler_->exportFile(write_filepath, "XML");
  } catch (const QString &msg) {
    QMessageBox::warning(this, tr("Warning"), msg);
    return;
  }

  QMessageBox::information(nullptr, tr("Export"), tr("Export Successful!"));
}

void PinHelper::on_explicit_clock_checkbox_stateChanged(int state) {
  if (state == Qt::CheckState::Checked) {
    explicit_clock_line_edit_->setHidden(true);
    explicit_clock_label_->setHidden(true);
  } else {
    explicit_clock_line_edit_->setText("clk");
    explicit_clock_line_edit_->setHidden(false);
    explicit_clock_label_->setHidden(false);
  }
}

void PinHelper::on_module_selected(const QString &module_name) {
  auto_assign_handler_->setModuleName(module_name);
}
