#ifndef DEVICE_PIN_READER_H
#define DEVICE_PIN_READER_H

#include <QFile>
#include <QList>
#include <QObject>
#include <QString>


namespace pinhelper {

/// @brief Class for the device pin reader. It can read the ports from XML file
/// which contains the device information.
class DevicePinReader : public QObject {
  Q_OBJECT
 public:
  /// @brief Constructor.
  /// @param parent Parent widget.
  DevicePinReader(QObject* parent = nullptr);

  /// @brief Destructor.
  ~DevicePinReader();

  /// @brief Read the device information from the file.
  /// @param filepath Path of Device information file.
  /// @return True if the file is read successfully.
  bool readDeviceInformation(const QString& filepath);

  /// @brief Get the input pins.
  /// @return List of input pins.
  const QList<QString>& getInputPins() const;
  
  /// @brief Get the output pins.
  /// @return List of output pins.
  const QList<QString>& getOutputPins() const;

  /// @brief Get the clock pin.
  /// @return Clock pin.
  const QString& getClockPin() const;

 private:
  QList<QString> input_pins_;
  QList<QString> output_pins_;
  QString clock_pin_;
};

};  // namespace pinhelper

#endif  // DEVICE_PIN_READER_H
