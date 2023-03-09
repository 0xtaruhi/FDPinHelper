#include "RegExpPortExtract.h"

#include <iostream>
#include <tuple>

using namespace ufde::auto_assignment;

VerilogPortDirection getPortDirection(const QString &port_direction) {
  if (port_direction == "input") {
    return VerilogPortDirection::INPUT;
  } else if (port_direction == "output") {
    return VerilogPortDirection::OUTPUT;
  } else if (port_direction == "inout") {
    return VerilogPortDirection::INOUT;
  } else {
    return VerilogPortDirection::UNKNOWN;
  }
}

VerilogPortRange getPortRange(const QString &port_range) {
  int upper = 0;
  int lower = 0;

  if (port_range.isEmpty()) {
    upper = 0;
    lower = 0;
  } else {
    auto range_str(port_range);
    if (range_str.startsWith("[")) {
      range_str.remove(0, 1);
    }
    if (range_str.endsWith("]")) {
      range_str.remove(range_str.size() - 1, 1);
    }

    QStringList port_range_list = range_str.split(":");
    if (port_range_list.size() != 2) {
      throw QString("Port range error: %1").arg(port_range);
    }
    try {
      upper = port_range_list[0].toInt();
      lower = port_range_list[1].toInt();
    } catch (...) {
      throw QString("Port range error: %1").arg(port_range);
    }
  }

  return VerilogPortRange(upper, lower);
}

RegExpPortExtract::RegExpPortExtract(QObject *parent) : QObject(parent) {}

RegExpPortExtract::~RegExpPortExtract() {}

void RegExpPortExtract::setFile(const QString &filename) {
  filename_ = filename;
  module_names_.clear();
}

const QStringList &RegExpPortExtract::getModuleNames() const {
  return module_names_;
}

void RegExpPortExtract::resolveModules() {
  QFile file(filename_);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    throw QString("Can't open file: %1").arg(filename_);
    return;
  }

  QString file_content = file.readAll();
  file.close();
  QRegularExpression reg_exp("\\bmodule\\s+(\\w+)");
  QRegularExpressionMatchIterator reg_exp_match_iterator =
      reg_exp.globalMatch(file_content);

  while (reg_exp_match_iterator.hasNext()) {
    QRegularExpressionMatch reg_exp_match = reg_exp_match_iterator.next();
    module_names_.append(reg_exp_match.captured(1));
  }
}

void RegExpPortExtract::resolvePorts(const QString &module_name) {
  QFile file(filename_);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    throw QString("Can't open file: %1").arg(filename_);
    return;
  }

  // Read file content
  QString file_content = file.readAll();
  file.close();

  // Get module content
  // module ... endmodule
  QRegularExpression reg_exp_module_content(
      QString("\\bmodule\\s+%1.*?endmodule").arg(module_name),
      QRegularExpression::DotMatchesEverythingOption);

  QRegularExpressionMatch reg_exp_match_module_content =
      reg_exp_module_content.match(file_content);

  if (!reg_exp_match_module_content.hasMatch()) {
    throw QString("Can't find module: %1").arg(module_name);
    return;
  }

  QString module_content = reg_exp_match_module_content.captured(0);

  // Remove comments
  // Comments 1 // ... \n
  QRegularExpression reg_exp_comments(
      QString("//.*?\\n"), QRegularExpression::DotMatchesEverythingOption);
  module_content.remove(reg_exp_comments);
  // Comment 2 /* ... */
  QRegularExpression reg_exp_comments2(
      QString("/\\*.*?\\*/"), QRegularExpression::DotMatchesEverythingOption);
  module_content.remove(reg_exp_comments2);

  // Clear module ports
  module_ports_.insert(module_name, QList<VerilogPort>());
  auto &module_ports = module_ports_[module_name];

  // Get ports
  // input|output|inout [width] [name]
  QRegularExpression reg_exp_ports(
      QString("(input|output|inout)\\s+((reg|wire|logic)\\s+)?(\\[\\d+:\\d+\\]"
              "\\s+)?"
              "((\\w+\\s*,\\s*)*\\w+\\s*[;,\\)])"),
      QRegularExpression::DotMatchesEverythingOption);

  QRegularExpressionMatchIterator reg_exp_match_iterator_ports =
      reg_exp_ports.globalMatch(module_content);

  while (reg_exp_match_iterator_ports.hasNext()) {
    QRegularExpressionMatch reg_exp_match_ports =
        reg_exp_match_iterator_ports.next();

    const auto &decl = reg_exp_match_ports.captured(0);

    // Get direction, width and name
    auto direction_str = reg_exp_match_ports.captured(1).trimmed();
    auto width_str = reg_exp_match_ports.captured(4).trimmed();
    auto name_str = reg_exp_match_ports.captured(5).trimmed();

    auto direction = getPortDirection(direction_str);
    VerilogPortRange width;
    try {
      width = getPortRange(width_str);
    } catch (const QString &e) {
      throw QString("Module: %1, Port Declaration: %2, Error: %3")
          .arg(module_name)
          .arg(decl)
          .arg(e);
    }

    // remove last comma or semicolon or bracket
    if (name_str.endsWith(",") || name_str.endsWith(";") ||
        name_str.endsWith(")")) {
      name_str.chop(1);
    }
    QStringList name_list = name_str.split(",");
    for (auto &name : name_list) {
      name = name.trimmed();
      module_ports.emplace_back(name, direction, width);
    }
  }
}

QList<VerilogPort> RegExpPortExtract::getPorts(
    const QString &module_name) const {
  return module_ports_[module_name];
}

const QMap<QString, QList<VerilogPort>> &RegExpPortExtract::getModulePorts()
    const {
  return module_ports_;
}

QMap<QString, QList<VerilogPort>> &RegExpPortExtract::getModulePorts() {
  return module_ports_;
}