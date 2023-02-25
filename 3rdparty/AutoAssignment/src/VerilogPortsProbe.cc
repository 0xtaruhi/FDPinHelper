#include "VerilogPortsProbe.h"
#include "VerilogPort.h"
#include "verilog_ast_common.h"
#include "verilog_ast_util.h"
#include "verilog_parser.h"
#include "verilog_preprocessor.h"

using namespace ufde::auto_assignment;
using uint = unsigned int;

VerilogPortsProbe::VerilogPortsProbe()
    : file_handler_(nullptr), filename_(nullptr) {
  verilog_parser_init();
}

VerilogPortsProbe::VerilogPortsProbe(const std::string &filename)
    : VerilogPortsProbe() {
  setFile(filename);
}

VerilogPortsProbe::~VerilogPortsProbe() {
  if (file_handler_ != nullptr) {
    fclose(file_handler_);
  }
}

void VerilogPortsProbe::setFile(const std::string &filename) {
  if (file_handler_ != nullptr) {
    fclose(file_handler_);
    delete[] filename_;
    filename_ = nullptr;
  }

  file_handler_ = fopen(filename.c_str(), "r");
  if (file_handler_ == nullptr) {
    throw std::runtime_error("Could not open file " + filename);
  } else {
    filename_ = new char[filename.size() + 1];
    std::copy(filename.begin(), filename.end(), filename_);
    filename_[filename.size()] = '\0';
  }
}

void VerilogPortsProbe::resolve() {
  if (file_handler_ == nullptr) {
    throw std::runtime_error("No file is opened.");
  }
  
  verilog_preprocessor_set_file(yy_preproc, filename_);

  auto parse_result = verilog_parse_file(file_handler_);
  if (parse_result != 0) {
    throw std::runtime_error("Parse error." + std::to_string(parse_result));
  }

  verilog_source_tree *ast = yy_verilog_source_tree;

  verilog_resolve_modules(ast);

  auto modules_num = ast->modules->items;
  for (uint i = 0; i != modules_num; ++i) {
    auto module =
        static_cast<ast_module_declaration *>(ast_list_get(ast->modules, i));
    auto module_name = module->identifier->identifier;
    modules_.insert({module_name, extractPorts(module)});
  }
}

VerilogPortsProbe::PortsVec
VerilogPortsProbe::getPorts(const std::string &module_name) const {
  auto it = modules_.find(module_name);
  if (it == modules_.end()) {
    throw std::runtime_error("Module " + module_name + " not found.");
  }
  return it->second;
}

std::vector<std::string> VerilogPortsProbe::getModuleNames() const {
  std::vector<std::string> module_names;
  for (auto &module : modules_) {
    module_names.push_back(module.first);
  }
  return module_names;
}

const std::map<std::string, VerilogPortsProbe::PortsVec> &
VerilogPortsProbe::getModulesWithPorts() const {
  return modules_;
}

auto VerilogPortsProbe::extractPorts(ast_module_declaration *module)
    -> PortsVec {

  PortsVec ports;

  auto filterNumber = [](const char *str) -> int {
    int num = 0;
    bool is_negative = false;
    if (str && *str == '-') {
      is_negative = true;
      ++str;
    }
    while (*str != '\0') {
      if (*str >= '0' && *str <= '9') {
        num = num * 10 + (*str - '0');
      }
      ++str;
    }
    return is_negative ? -num : num;
  };

  uint ports_decl_num = module->module_ports->items;
  for (uint i = 0; i != ports_decl_num; ++i) {
    auto decl_port = static_cast<ast_port_declaration *>(
        ast_list_get(module->module_ports, i));

    auto cur_decl_ports_num = decl_port->port_names->items;
    auto ast_direction = decl_port->direction;
    auto direction = getPortDirectionByAstDefinition(ast_direction);

    auto range = decl_port->range;
    int upper = 0, lower = 0;
    if (range != nullptr) {
      upper = filterNumber(ast_expression_tostring(range->upper));
      lower = filterNumber(ast_expression_tostring(range->lower));
    }

    for (uint j = 0; j != cur_decl_ports_num; ++j) {
      auto port_name_id =
          static_cast<ast_identifier>(ast_list_get(decl_port->port_names, j));
      auto port_name = std::string(port_name_id->identifier);
      ports.emplace_back(port_name, direction, VerilogPortRange(upper, lower));
    }
  }

  return ports;
}
