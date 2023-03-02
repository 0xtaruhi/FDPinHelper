#include "AutoAssignHandler.h"

#include "DevicePinReader.h"

using namespace pinhelper;

AutoAssignHandler::AutoAssignHandler(QObject* parent) : QObject(parent) {
  ports_prober_ = new VerilogPortsProbe();
}

AutoAssignHandler::~AutoAssignHandler() { delete ports_prober_; }

void AutoAssignHandler::setFilepath(const QString& filepath) {
  if (filepath.isEmpty()) {
    throw QString(tr("Filepath is empty"));
    return;
  }
  if (!QFile::exists(filepath)) {
    throw QString(tr("Filepath does not exist"));
    return;
  }
  filepath_ = filepath.toStdString();
}

void AutoAssignHandler::setModuleName(const QString& module_name) {
  if (module_name.isEmpty()) {
    throw QString(tr("Module name is empty"));
    return;
  }
  module_name_ = module_name.toStdString();
  resolvePorts();
}

void AutoAssignHandler::resolveModules() {
  ports_prober_->setFile(filepath_);
  try {
    ports_prober_->resolve();
  } catch (std::runtime_error err) {
    throw QString(err.what());
    return;
  }

  const auto& module_names = ports_prober_->getModuleNames();
  if (module_names.size() == 0) {
    throw QString(tr("No module found"));
    return;
  }

  if (module_names.size() == 1) {
    module_name_ = module_names[0];
    resolvePorts();
    return;
  }

  QStringList module_names_qt;
  for (const auto& module_name : module_names) {
    module_names_qt.append(QString::fromStdString(module_name));
  }
  emit multipleModulesFound(module_names_qt);
}

void AutoAssignHandler::resolvePorts() {
  ports_.clear();
  auto ports_from_prober = ports_prober_->getPorts(module_name_);

  for (const auto& port : ports_from_prober) {
    if (port.range.isSingleBit()) {
      ports_.push_back(port);
    } else {
      auto& range = port.range;
      auto step = range.upper > range.lower ? 1 : -1;
      for (auto i = range.lower; i != range.upper + step; i += step) {
        VerilogPort port_with_range = port;
        port_with_range.name += "[" + std::to_string(i) + "]";
        ports_.push_back(port_with_range);
      }
    }
  }
}

auto AutoAssignHandler::getPorts() const noexcept -> const QList<VerilogPort>& {
  return ports_;
}

auto AutoAssignHandler::defaultAssign(const QList<VerilogPort>& ports,
                                      const QString& clk_name,
                                      const QString& device_name)
    -> QList<PinTableWidget::PinItem> {
  QList<PinTableWidget::PinItem> pin_items;
  DevicePinReader device_pin_reader;
  device_pin_reader.readDeviceInformation(
      QString(":/device/%1.xml").arg(device_name));

  const auto& input_pins = device_pin_reader.getInputPins();
  const auto& output_pins = device_pin_reader.getOutputPins();
  const auto& clock_pin = device_pin_reader.getClockPin();

  auto input_pins_it = input_pins.begin();
  auto output_pins_it = output_pins.begin();

  using PinItem = PinTableWidget::PinItem;
  using VerilogPortDirection = ufde::auto_assignment::VerilogPortDirection;

  bool clock_found = false;

  for (const auto& port : ports) {
    if (port.name == clk_name.toStdString()) {
      if (clock_found) {
        throw QString(tr("Multiple clock pins found"));
        return {};
      }
      pin_items.push_back(PinItem(QString::fromStdString(port.name),
                                  PinItem::Direction::Input, clock_pin));
      clock_found = true;
      continue;
    } else if (port.direction == VerilogPortDirection::INPUT) {
      if (input_pins_it == input_pins.end()) {
        throw QString("Not enough input pins, max input pins: %1")
            .arg(input_pins.size());
      }
      pin_items.push_back(PinItem(QString::fromStdString(port.name),
                                  PinItem::Direction::Input, *input_pins_it++));
    } else if (port.direction == VerilogPortDirection::OUTPUT) {
      if (output_pins_it == output_pins.end()) {
        throw QString("Not enough output pins, max output pins: %1")
            .arg(output_pins.size());
      }
      pin_items.push_back(PinItem(QString::fromStdString(port.name),
                                  PinItem::Direction::Output,
                                  *output_pins_it++));
    }
  }
  return pin_items;
}
