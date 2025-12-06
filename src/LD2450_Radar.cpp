/**
 * @file       LD2450_Radar.cpp
 * @brief      An Arduino library that makes it easy to configure and use
 *                the HiLink 24GHz millimeter-wave Human Presence Detection
 *                sensor (LD2450).
 * @license    The MIT License (MIT)
 */

#include <Arduino.h>
#include "LD2450_Radar.h"
#include "helpers.h"
#include <format>

using namespace std;

const vector<uint8_t> COMMAND_HEADER = {0xFD, 0xFC, 0xFB, 0xFA};
const vector<uint8_t> COMMAND_TAIL = {0x04, 0x03, 0x02, 0x01};

const vector<uint8_t> REPORT_HEADER = {0xAA, 0xFF, 0x03, 0x00};
const vector<uint8_t> REPORT_TAIL = {0x55, 0xCC};

const uint8_t MIN_RESPONSE_BYTES = 14;

LD2450_Radar::LD2450_Radar(HardwareSerial* _ser) {
  ser = _ser;
}

/**
 * @brief Send a command to the radar (see docs 2.1.2)
 * 
 * @param intraFrameLength the intra frame length
 * @param commandWord the command word
 * @param commandValue the command value
 * @return byte[] the response from the radar
 */
vector<uint8_t> LD2450_Radar::sendCommand(const vector<uint8_t>& commandWord, const vector<uint8_t>& commandValue) {
  if (!ser->availableForWrite()) {
    Serial.print("Unable to send command, serial port is unavailable");
    return vector<uint8_t>{};
  }
  // Serial.printf("Command word: %s\n", bytes_to_hex(commandWord).c_str());
  // Serial.printf("Command value: %s\n", bytes_to_hex(commandValue).c_str());
  auto frame = concatenate_vectors<uint8_t>({commandWord, commandValue});
  // Serial.printf("Frame: %s\n", bytes_to_hex(frame).c_str());
  auto intraFrameLength = to_little_endian<uint16_t>(frame.size());
  // Serial.printf("Length: %s\n", bytes_to_hex(intraFrameLength).c_str());
  auto command = concatenate_vectors<uint8_t>({COMMAND_HEADER, intraFrameLength, frame, COMMAND_TAIL});
  Serial.printf("Command [%d]: %s\n", command.size(), bytes_to_hex(command).c_str());
  ser->write(&command[0], command.size());
  ser->flush();
  return read_until(ser, COMMAND_TAIL);
}

/**
 * @brief Check if the command was sent successfully
 * 
 * @param response the response from the radar
 * @return true if the command was sent successfully
 * @return false otherwise
 */
bool LD2450_Radar::getCommandSuccess(const vector<uint8_t>& response) {
  if (response.size() < MIN_RESPONSE_BYTES) return false;
  auto success = from_little_endian<uint16_t>(span(response.begin() + 8, 2));
  return success == 0 ? true : false;
}

/**
 * @brief Set the radar to configuration mode (see docs 2.2.1)
 * 
 * @return true if the configuration mode was successfully enabled
 * @return false otherwise
 */
bool LD2450_Radar::beginConfigurationMode() {
  vector<uint8_t> commandWord = {0xFF, 0x00};
  vector<uint8_t> commandValue = {0x01, 0x00};
  auto response = sendCommand(commandWord, commandValue);
  auto commandSuccessful = getCommandSuccess(response);
  if (commandSuccessful) {
    // print "Configuration mode enabled"
  } else {
    // print "Configuration enable failed"
  }
  return commandSuccessful;
}

/**
 * @brief End the configuration mode (see docs 2.2.2)
 * 
 * @return true if the configuration mode was successfully ended
 * @return false otherwise
 */
bool LD2450_Radar::endConfigurationMode() {
  vector<uint8_t> commandWord = {0xFE, 0x00};
  vector<uint8_t> commandValue = {};
  auto response = sendCommand(commandWord, commandValue);
  auto commandSuccessful = getCommandSuccess(response);
  if (commandSuccessful) {
    // print "Configuration mode disabled"
  } else {
    // print "Configuration disable failed"
  }
  return commandSuccessful;
}

/**
 * @brief Set the radar to single target tracking mode (see docs 2.2.3)
 * 
 * @return true if the single target tracking mode was successfully enabled
 * @return false otherwise
 */
bool LD2450_Radar::setSingleTargetTracking() {
  vector<uint8_t> commandWord = {0x80, 0x00};
  vector<uint8_t> commandValue = {};
  auto response = sendCommand(commandWord, commandValue);
  auto commandSuccessful = getCommandSuccess(response);
  if (commandSuccessful) {
    // print "Single target tracking mode enabled"
  } else {
    // print "Single target tracking mode enable failed"
  }
  return commandSuccessful;
}

/**
 * @brief Set the radar to multi target tracking mode (see docs 2.2.4)
 * 
 * @return true if the multiple target tracking mode was successfully enabled
 * @return false otherwise
 */
bool LD2450_Radar::setMultiTargetTracking() {
  vector<uint8_t> commandWord = {0x90, 0x00};
  vector<uint8_t> commandValue = {};
  auto response = sendCommand(commandWord, commandValue);
  auto commandSuccessful = getCommandSuccess(response);
  if (commandSuccessful) {
    // print "Multi target tracking mode enabled"
  } else {
    // print "Multi target tracking mode enable failed"
  }
  return commandSuccessful;
}

/**
 * @brief Query the target tracking mode, the default mode is multi target tracking (see docs 2.2.5)
 * 
 * @return int 1 for single target tracking, 2 for multi target tracking
 */
uint16_t LD2450_Radar::getTargetTrackingMode() {
  vector<uint8_t> commandWord = {0x91, 0x00};
  vector<uint8_t> commandValue = {};
  auto response = sendCommand(commandWord, commandValue);
  auto commandSuccessful = getCommandSuccess(response);
  if (commandSuccessful) {
    uint16_t trackingMode = from_little_endian<uint16_t>(span(response.begin() + 10, 2));
    // printf "Tracking mode: {trackingMode}"
    return trackingMode;
  }
  // print "Query target tracking mode failed"
  return 0;
}

/**
 * @brief Read the firmware version of the radar (see docs 2.2.6)
 * 
 * @return string with the firmware version of the radar
 */
string LD2450_Radar::getFirmwareVersion() {
  vector<uint8_t> commandWord = {0xA0, 0x00};
  vector<uint8_t> commandValue = {};
  auto response = sendCommand(commandWord, commandValue);
  auto commandSuccessful = getCommandSuccess(response);
  if (commandSuccessful) {
    uint8_t firmwareType = from_little_endian<uint8_t>(span(response.begin() + 13, 1));
    uint8_t majorNumber = from_little_endian<uint8_t>(span(response.begin() + 12, 1));
    uint32_t minorNumber = from_little_endian<uint32_t>(span(response.begin() + 14, 4));
    auto firmwareVersion = format("V{}.{}.{}", firmwareType, majorNumber, minorNumber);
    // printf "Firmware version: {firmwareVersion}"
    return firmwareVersion;
  }
  Serial.println("Get firmware version failed");
  return "";
}

/**
 * @brief Set the serial port baud rate of the radar (see docs 2.2.7)
 * 
 * @param baudRate the baud rate of the radar
 * @return true if the baud rate was successfully set
 * @return false otherwise
 */
bool LD2450_Radar::setSerialPortSpeed(BaudRates baudRate) {
  uint16_t _baudRate = (uint16_t) baudRate;
  vector<uint8_t> commandWord = {0xA1, 0x00};
  vector<uint8_t> commandValue = to_little_endian<uint16_t>(_baudRate);
  auto response = sendCommand(commandWord, commandValue);
  auto commandSuccessful = getCommandSuccess(response);
  if (commandSuccessful) {
    // print "Serial port baud rate set to {baudRate}"
  } else {
    // print "Set serial port baud rate failed"
  }
  return commandSuccessful;
}

/**
 * @brief Restore the factory settings of the radar (see docs 2.2.8)
 * 
 * @return true if the factory settings were successfully restored
 * @return false otherwise
 */
bool LD2450_Radar::restoreFactorySettings() {
  vector<uint8_t> commandWord = {0xA2, 0x00};
  vector<uint8_t> commandValue = {};
  auto response = sendCommand(commandWord, commandValue);
  auto commandSuccessful = getCommandSuccess(response);
  if (commandSuccessful) {
    // print "Factory settings restored"
  } else {
    // print "Restore factory settings failed"
  }
  return commandSuccessful;
}

/**
 * @brief Restart the radar module (see docs 2.2.9)
 * 
 * @return true if the radar module was successfully restarted
 * @return false otherwise
 */
bool LD2450_Radar::restart() {
  vector<uint8_t> commandWord = {0xA3, 0x00};
  vector<uint8_t> commandValue = {};
  Serial.print("Restarting module... ");
  auto response = sendCommand(commandWord, commandValue);
  auto commandSuccessful = getCommandSuccess(response);
  Serial.print(commandSuccessful ? "OK" : "Fail");
  return commandSuccessful;
}

/**
 * @brief Set up the bluetooth of the radar (see docs 2.2.10)
 * 
 * @param isOn true if the bluetooth should be enabled, false otherwise
 * @return true if the bluetooth was successfully set up
 * @return false otherwise
 */
bool LD2450_Radar::setBluetooth(bool isOn) {
  vector<uint8_t> commandWord = {0xA4, 0x00};
  vector<uint8_t> commandValue = isOn ? vector<uint8_t>{0x01, 0x00} : vector<uint8_t>{0x00, 0x00};
  auto response = sendCommand(commandWord, commandValue);
  auto commandSuccessful = getCommandSuccess(response);
  if (commandSuccessful) {
    // print "Bluetooth {isOn ? "enabled" : "disabled"}"
  } else {
    // print "Bluetooth setup failed"
  }
  return commandSuccessful;
}

/**
 * @brief Get the MAC address of the radar (see docs 2.2.11)
 * 
 * @return String the MAC address of the radar
 */
string LD2450_Radar::getMacAddress() {
  vector<uint8_t> commandWord = {0xA5, 0x00};
  vector<uint8_t> commandValue = {0x01, 0x00};
  auto response = sendCommand(commandWord, commandValue);
  auto commandSuccessful = getCommandSuccess(response);
  if (commandSuccessful) {
    auto bytes = span<uint8_t>(response.begin() + 10, response.begin() + 10 + 12);
    return string(bytes.begin(), bytes.end());
  }
  // print "Get MAC address failed"
  return "";
}

/**
 * @brief Query the current zone filtering mode of the radar (see docs 2.2.12)
 * 
 * @return ZoneConfig structure containing zone filtering mode and regions configuration
 */
ZoneConfig LD2450_Radar::getZoneConfig() {
  vector<uint8_t> commandWord = {0xC1, 0x00};
  vector<uint8_t> commandValue = {};
  auto response = sendCommand(commandWord, commandValue);
  auto commandSuccessful = getCommandSuccess(response);
  if (commandSuccessful) {
    ZoneConfig zoneConfig = {
      from_little_endian<int16_t>(span(response.begin() + 10, 2)),      // zoneFilteringMode
      Region{
        from_little_endian<int16_t>(span(response.begin() + 12, 2)),     // x1
        from_little_endian<int16_t>(span(response.begin() + 14, 2)),     // y1
        from_little_endian<int16_t>(span(response.begin() + 16, 2)),     // x2
        from_little_endian<int16_t>(span(response.begin() + 18, 2)),     // y2
      },
      Region{
        from_little_endian<int16_t>(span(response.begin() + 20, 2)),     // x1
        from_little_endian<int16_t>(span(response.begin() + 22, 2)),     // y1
        from_little_endian<int16_t>(span(response.begin() + 24, 2)),     // x2
        from_little_endian<int16_t>(span(response.begin() + 26, 2)),     // y2
      },
      Region{
        from_little_endian<int16_t>(span(response.begin() + 28, 2)),     // x1
        from_little_endian<int16_t>(span(response.begin() + 30, 2)),     // y1
        from_little_endian<int16_t>(span(response.begin() + 32, 2)),     // x2
        from_little_endian<int16_t>(span(response.begin() + 34, 2)),     // y2
      },
    };
    // printf "Zone filtering mode: {zoneConfig.zoneFilteringMode}"
    return zoneConfig;
  }
  // print "Query zone filtering mode failed"
  return ZoneConfig{};
}

/**
 * @brief Set the zone filtering mode of the radar (see docs 2.2.13)
 * 
 * @param zoneConfig structure containing zone filtering mode and regions configuration
 * @return true if the zone filtering mode was successfully set
 * @return false otherwise
 */
bool LD2450_Radar::setZoneConfig(ZoneConfig& zoneConfig) {
  vector<uint8_t> commandWord = {0xC2, 0x00};
  vector<uint8_t> commandValue = concatenate_vectors<uint8_t>({
    to_little_endian<int16_t>(zoneConfig.zoneFilteringMode),
    to_little_endian<int16_t>(zoneConfig.region1.x1),
    to_little_endian<int16_t>(zoneConfig.region1.y1),
    to_little_endian<int16_t>(zoneConfig.region1.x2),
    to_little_endian<int16_t>(zoneConfig.region1.y2),
    to_little_endian<int16_t>(zoneConfig.region2.x1),
    to_little_endian<int16_t>(zoneConfig.region2.y1),
    to_little_endian<int16_t>(zoneConfig.region2.x2),
    to_little_endian<int16_t>(zoneConfig.region2.y2),
    to_little_endian<int16_t>(zoneConfig.region3.x1),
    to_little_endian<int16_t>(zoneConfig.region3.y1),
    to_little_endian<int16_t>(zoneConfig.region3.x2),
    to_little_endian<int16_t>(zoneConfig.region3.y2)
  });
  auto response = sendCommand(commandWord, commandValue);
  auto commandSuccessful = getCommandSuccess(response);
  if (commandSuccessful) {
    // printf "Zone filtering mode set to {zoneConfig.zoneFilteringMode}"
  } else {
    // print "Set zone filtering mode failed"
  }
  return commandSuccessful;
}

Target createTarget(span<const uint8_t> bytes) {
  return Target{
    to_signed(from_little_endian<uint16_t>(bytes.subspan(2, 2))),  // x
    to_signed(from_little_endian<uint16_t>(bytes.subspan(0, 2))),  // y
    to_signed(from_little_endian<uint16_t>(bytes.subspan(4, 2))),  // speed
    from_little_endian<uint16_t>(bytes.subspan(6, 2))              // distanceResolution
  };
}

PositionData createEmptyTargets() {
  return PositionData{
    Target{0, 0, 0, 0},
    Target{0, 0, 0, 0},
    Target{0, 0, 0, 0}
  };
}

void printTarget(const Target& t, int index) {
  if (!t.x && !t.y && !t.speed) return;
  Serial.printf("Target %d: {x: %d mm, y: %d mm, speed: %d cm/s, distance res: %d mm}\n", index, t.x, t.y, t.speed, t.distanceResolution);
}

void LD2450_Radar::loop() {
  if (ser->available() == 0) return;
  auto targetBytes = read_until(ser, REPORT_TAIL);
  auto pd = getTargetTrackingData(targetBytes);
  printTarget(pd.target1, 1);
  printTarget(pd.target2, 2);
  printTarget(pd.target3, 3);
}

/**
 * @brief Read the basic mode data from the serial port line (see docs 2.3)
 * 
 * @param serialData the serial port line
 * @return the radar data
 */
PositionData LD2450_Radar::getTargetTrackingData(const vector<uint8_t>& serialData) {
  if (contains<uint8_t>(serialData, REPORT_HEADER) && contains<uint8_t>(serialData, REPORT_TAIL)) {
    // Interpret target data
    if (serialData.size() == 30) {
      return PositionData{
        createTarget(span(serialData.begin() + 4, 8)),
        createTarget(span(serialData.begin() + 12, 8)),
        createTarget(span(serialData.begin() + 20, 8)),
      };
    }
    // if the target data is not 30 bytes long the line is corrupted.
    Serial.println("Serial port line corrupted - not 30 bytes long");
  }
  // if the header and tail are not present the line is corrupted.
  // print "Serial port line corrupted - header or tail not present"
  return createEmptyTargets();
}
