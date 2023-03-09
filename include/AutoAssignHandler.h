#ifndef AUTO_ASSIGN_HANDLER_H
#define AUTO_ASSIGN_HANDLER_H

#include <QFile>
#include <QObject>
#include <Qstring>

#include "PinTableWidget.h"
#include "RegExpPortExtract.h"
#include "VerilogPort.h"

namespace pinhelper {

/// @brief Class. It contains the logic for auto assigning the pins.
class AutoAssignHandler : public QObject {
  Q_OBJECT

  using RegExpPortExtract = ufde::auto_assignment::RegExpPortExtract;
  using VerilogPort = ufde::auto_assignment::VerilogPort;

public:
  /// @brief Constructor.
  /// @param parent Parent widget.
  AutoAssignHandler(QObject *parent = nullptr);

  /// @brief Destructor.
  ~AutoAssignHandler();

  /// @brief Set the filepath of the verilog file.
  /// @param filepath Path of the verilog file.
  void setFilepath(const QString &filepath);

  /// @brief Set the module name.
  /// @note Module name is always autimatically set when the verilog file has
  /// only one module. If the verilog file has multiple modules,
  /// multipleModulesFound Signal is emitted and the module name is set by the
  /// user. Thus, this function should be **only called** when the verilog file
  /// has multiple modules.
  /// @param module_name Name of the module.
  void setModuleName(const QString &module_name);

  /// @brief Resolve the verilog file. It has following steps:
  /// 1. Probe the verilog file and get all modules.
  /// 2. If there is only one module, call resolvePorts function.
  /// 3. If there are multiple modules, then emit multipleModulesFound signal.
  /// When the user selects the module, call resolvePorts function.
  void resolveModules();

  /// @brief Get all ports of the module.
  /// @note This function should be called after resolveModules function. If
  /// multiple modules are found, then the user should select the module by
  /// calling setModuleName function first.
  /// @return List of ports.
  const QList<VerilogPort> &getPorts() const noexcept;

  static QList<PinTableWidget::PinItem>
  defaultAssign(const QList<VerilogPort> &ports,
                const QString &clk_name = "clk",
                const QString &device_name = "FDP3P7");

signals:
  /// @brief Signal. Emitted when multiple modules are found in the verilog
  /// file.
  void multipleModulesFound(QStringList modules);

private:
  void resolvePorts();

private:
  QString module_name_; ///< Name of the module.

  QList<VerilogPort> ports_;    ///< List of ports.
  RegExpPortExtract* extracter_; ///< RegExpPortExtract instance
};

} // namespace pinhelper

#endif // AUTO_ASSIGN_HANDLER_H
