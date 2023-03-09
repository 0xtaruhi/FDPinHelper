#ifndef VERILOG_PORT_H
#define VERILOG_PORT_H

#include <QString>

namespace ufde::auto_assignment {

enum class VerilogPortDirection {
  UNKNOWN,
  INPUT,
  OUTPUT,
  INOUT,
};

struct VerilogPortRange {
  VerilogPortRange() : upper(0), lower(0) {}
  VerilogPortRange(int upper, int lower) : upper(upper), lower(lower) {}

  bool isSingleBit() const { return upper == lower; }

  int upper;
  int lower;
};

struct VerilogPort {
  VerilogPort() : name(""), direction(VerilogPortDirection::UNKNOWN), range() {}
  VerilogPort(const QString &name, VerilogPortDirection direction,
              VerilogPortRange range)
      : name(name), direction(direction), range(range) {}

  QString name;
  VerilogPortDirection direction;
  VerilogPortRange range;
};

} // namespace ufde::auto_assignment

#endif // VERILOG_PORT_H
