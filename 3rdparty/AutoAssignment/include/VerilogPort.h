#ifndef VERILOG_PORT_H
#define VERILOG_PORT_H

#include "verilog_ast.h"
#include <string>

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

  int upper;
  int lower;
};

struct VerilogPort {
  VerilogPort() : name(""), direction(VerilogPortDirection::UNKNOWN), range() {}
  VerilogPort(const std::string &name, VerilogPortDirection direction,
              VerilogPortRange range)
      : name(name), direction(direction), range(range) {}

  std::string name;
  VerilogPortDirection direction;
  VerilogPortRange range;
};

inline constexpr VerilogPortDirection
getPortDirectionByAstDefinition(ast_port_direction ast_direction) {
  switch (ast_direction) {
  case PORT_INPUT:
    return VerilogPortDirection::INPUT;
  case PORT_OUTPUT:
    return VerilogPortDirection::OUTPUT;
  case PORT_INOUT:
    return VerilogPortDirection::INOUT;
  default:
    return VerilogPortDirection::UNKNOWN;
  }
}

} // namespace ufde::auto_assignment

#endif // VERILOG_PORT_H
