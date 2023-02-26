#include "PinHelper.h"

#include <QFile>
#include <QFileDialog>
#include <QGroupBox>
#include <QLayout>
#include <QMessageBox>
#include <QResizeEvent>
#include <string>

#include "FlatUI.h"
#include "ModulesSelectDialog.h"

constexpr auto kDefaultWidth = 500;
constexpr auto kDefaultHeight = 300;
constexpr auto kMinimumWidth = 400;
constexpr auto kMinimumHeight = 300;

using namespace pinhelper;

PinHelper::PinHelper(QWidget *parent) : QMainWindow(parent) {
  initUi();
  initConnections();

  ports_prober_ = new ufde::auto_assignment::VerilogPortsProbe();
}

PinHelper::~PinHelper() { delete ports_prober_; }

QSize PinHelper::sizeHint() const { return {kDefaultWidth, kDefaultHeight}; }

void PinHelper::initUi() {
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
  settings_group->setFixedHeight(settings_layout->sizeHint().height() + 20);

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
}

void PinHelper::probeVerilogFile() {
  auto filepath = filepath_line_edit_->text();
  if (filepath.isEmpty()) {
    QMessageBox::warning(this, tr("Warning"), tr("Please select a file."));
    return;
  }
  // Check if file exists
  if (!QFile::exists(filepath)) {
    QMessageBox::warning(this, tr("Warning"), tr("File does not exist."));
    return;
  }

  auto filepath_stdstr = filepath.toStdString();
  ports_prober_->setFile(filepath_stdstr);
  ports_prober_->resolve();

  auto module_names = ports_prober_->getModuleNames();
  if (module_names.empty()) {
    QMessageBox::warning(this, tr("Warning"), tr("No module found."));
    return;
  }

  auto module_index = 0;

  auto module_num = module_names.size();
  if (module_num > 1) {
    auto module_select_dialog = new ModulesSelectDialog();
    for (const auto &module_name : module_names) {
      module_select_dialog->addItem(QString::fromStdString(module_name));
    }
    module_select_dialog->updateListWidget();

    auto itemSelected = [&module_index](const size_t index) {
      module_index = index;
    };
    auto rejected = [&module_index]() { module_index = -1; };

    connect(module_select_dialog, &ModulesSelectDialog::itemSelected,
            itemSelected);
    connect(module_select_dialog, &ModulesSelectDialog::rejected, rejected);
    module_select_dialog->exec();

    if (module_index == -1) {
      return;
    }
  }

  auto module_name = module_names[module_index];
  auto ports = ports_prober_->getPorts(module_name);

  auto directionTransform =
      [](const ufde::auto_assignment::VerilogPortDirection &direction) {
        switch (direction) {
          case ufde::auto_assignment::VerilogPortDirection::INPUT:
            return PinTableWidget::PinItem::Direction::Input;
          case ufde::auto_assignment::VerilogPortDirection::OUTPUT:
            return PinTableWidget::PinItem::Direction::Output;
          case ufde::auto_assignment::VerilogPortDirection::INOUT:
            return PinTableWidget::PinItem::Direction::Inout;
          default:
            return PinTableWidget::PinItem::Direction::Unknown;
        }
        return PinTableWidget::PinItem::Direction::Unknown;
      };

  auto addToPinTable = [&](const ufde::auto_assignment::VerilogPort &port) {
    PinTableWidget::PinItem pin_item;
    pin_item.setName(QString::fromStdString(port.name));
    pin_item.setDirection(directionTransform(port.direction));
    pin_table_->addItem(pin_item);
  };

  pin_table_->clear();

  for (const auto &port : ports) {
    if (port.range.isSingleBit()) {
      addToPinTable(port);
    } else {
      auto range = port.range;
      auto step = range.upper > range.lower ? 1 : -1;
      for (auto i = range.lower; i != range.upper + step; i += step) {
        auto port_copy = port;
        port_copy.name += "[" + std::to_string(i) + "]";
        addToPinTable(port_copy);
      }
    }
  }
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

  file_dialog->show();
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
