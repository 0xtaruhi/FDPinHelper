#ifndef REG_EXP_PORT_EXTRACT_H
#define REG_EXP_PORT_EXTRACT_H

#include <QFile>
#include <QList>
#include <QMap>
#include <QRegularExpression>
#include <QString>
#include <QStringList>

#include "VerilogPort.h"

namespace ufde::auto_assignment {

class RegExpPortExtract : public QObject {
  Q_OBJECT
 public:
  RegExpPortExtract(QObject *parent = nullptr);
  virtual ~RegExpPortExtract();

  /// @brief Method. Set File handler
  /// @param filename QString file name
  void setFile(const QString &filename);

  /// @brief Method. Get module names
  /// @return QStringList module names
  const QStringList &getModuleNames() const;

  /// @brief Method. Resolve Module Names
  void resolveModules();

  void resolvePorts(const QString &module_name);

  QList<VerilogPort> getPorts(const QString &module_name) const;

  const QMap<QString, QList<VerilogPort>> &getModulePorts() const;
  QMap<QString, QList<VerilogPort>> &getModulePorts();

 private:
  QString filename_;                                ///< File name
  QStringList module_names_;                        ///< Module names
  QMap<QString, QList<VerilogPort>> module_ports_;  ///< Module ports
};

}  // namespace ufde::auto_assignment

#endif  // REG_EXP_PORT_EXTRACT_H
