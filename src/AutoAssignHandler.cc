#include "AutoAssignHandler.h"

#include "DevicePinReader.h"

using namespace pinhelper;

AutoAssignHandler::AutoAssignHandler(QObject *parent) : QObject(parent) {
  extracter_ = new RegExpPortExtract(this);
}

AutoAssignHandler::~AutoAssignHandler() = default;

void AutoAssignHandler::setFilepath(const QString &filepath) {
  if (filepath.isEmpty()) {
    throw QString(tr("Filepath is empty"));
    return;
  }
  if (!QFile::exists(filepath)) {
    throw QString(tr("Filepath does not exist"));
    return;
  }
  extracter_->setFile(filepath);
}

void AutoAssignHandler::setDevicePinReader(DevicePinReader *device_reader) {
  if (device_reader == nullptr) {
    throw QString(tr("Device reader is null"));
    return;
  }
  device_pin_reader_ = device_reader;
}

void AutoAssignHandler::setModuleName(const QString &module_name) {
  if (module_name.isEmpty()) {
    throw QString(tr("Module name is empty"));
    return;
  }
  module_name_ = module_name;
  resolvePorts();
}

const QString &AutoAssignHandler::getModuleName() const noexcept {
  return module_name_;
}

void AutoAssignHandler::resolveModules() {
  try {
    extracter_->resolveModules();
  } catch (const QString &err) {
    throw err;
  }

  const auto &module_names = extracter_->getModuleNames();
  if (module_names.size() == 0) {
    throw QString(tr("No module found"));
    return;
  }

  if (module_names.size() == 1) {
    module_name_ = module_names[0];
    resolvePorts();
    return;
  }

  emit multipleModulesFound(module_names);
}

void AutoAssignHandler::resolvePorts() {
  ports_.clear();
  extracter_->resolvePorts(module_name_);
  auto ports_from_prober = extracter_->getPorts(module_name_);

  for (const auto &port : ports_from_prober) {
    if (port.range.isSingleBit()) {
      ports_.push_back(port);
    } else {
      auto &range = port.range;
      auto step = range.upper > range.lower ? 1 : -1;
      for (auto i = range.lower; i != range.upper + step; i += step) {
        VerilogPort port_with_range = port;
        port_with_range.name.append(QString("[%1]").arg(i));
        ports_.push_back(port_with_range);
      }
    }
  }
}

auto AutoAssignHandler::getPorts() const noexcept
    -> const QList<VerilogPort> & {
  return ports_;
}

auto AutoAssignHandler::defaultAssign(const QList<VerilogPort> &ports,
                                      const QString &clk_name,
                                      const QString &device_name)
    -> QList<PinTableWidget::PinItem> {
  QList<PinTableWidget::PinItem> pin_items;
  // device_pin_reader_->readDeviceInformation(
  //     QString(":/device/%1.xml").arg(device_name));

  const auto &input_pins = device_pin_reader_->getInputPins();
  const auto &output_pins = device_pin_reader_->getOutputPins();
  const auto &clock_pin = device_pin_reader_->getClockPin();

  auto input_pins_it = input_pins.begin();
  auto output_pins_it = output_pins.begin();

  using PinItem = PinTableWidget::PinItem;
  using VerilogPortDirection = ufde::auto_assignment::VerilogPortDirection;

  bool clock_found = false;

  for (const auto &port : ports) {
    if (port.name == clk_name) {
      if (clock_found) {
        throw QString(tr("Multiple clock pins found"));
        return {};
      }
      pin_items.push_back(
          PinItem(port.name, PinItem::Direction::Input, clock_pin));
      clock_found = true;
      continue;
    } else if (port.direction == VerilogPortDirection::INPUT) {
      if (input_pins_it == input_pins.end()) {
        throw QString("Not enough input pins, max input pins: %1")
            .arg(input_pins.size());
      }
      pin_items.push_back(
          PinItem(port.name, PinItem::Direction::Input, *input_pins_it++));
    } else if (port.direction == VerilogPortDirection::OUTPUT) {
      if (output_pins_it == output_pins.end()) {
        throw QString("Not enough output pins, max output pins: %1")
            .arg(output_pins.size());
      }
      pin_items.push_back(
          PinItem(port.name, PinItem::Direction::Output, *output_pins_it++));
    }
  }
  return pin_items;
}
