#ifndef VERILOG_PORTS_PROBE_H
#define VERILOG_PORTS_PROBE_H

#include "VerilogPort.h"
#include "verilog_ast.h"
#include <cstdlib>
#include <fstream>
#include <map>
#include <vector>

namespace ufde::auto_assignment {

class VerilogPortsProbe {
  using PortsVec = std::vector<VerilogPort>;

public:
  /// @brief Default constructor. No file is opened.
  VerilogPortsProbe();

  /// @brief Constructor. Open the file to be probed.
  /// @param filename The name of the file to be probed.
  VerilogPortsProbe(const std::string &filename);

  /// @brief Destructor. If the file is still open, close it.
  ~VerilogPortsProbe();

  /// @brief Method, Set the file to be probed and open it. If currently a file
  /// is open, close it.
  /// @param filename The name of the file to be probed.
  void setFile(const std::string &filename);

  /// @brief Method, Resolve the file and extract the ports.
  void resolve();

  /// @brief Method, Get the ports of a module.
  /// @param module_name The name of the module.
  /// @return The ports of the module.
  PortsVec getPorts(const std::string &module_name) const;

  /// @brief Method, Get all names of the modules.
  /// @return The names of the modules.
  std::vector<std::string> getModuleNames() const;

  /// @brief Method, Get all modules and their ports.
  /// @return The modules and their ports.
  const std::map<std::string, PortsVec> &getModulesWithPorts() const;

private:
  FILE *file_handler_;                      /// The file handler.
  char *filename_;                          /// The name of the file.
  std::map<std::string, PortsVec> modules_; /// The modules and their ports.

  /// @brief Private Method, Extract the ports from a module.
  /// @param module The module to be extracted.
  /// @return The ports of the module.
  static PortsVec extractPorts(ast_module_declaration *module);
};

} // namespace ufde::auto_assignment

#endif
