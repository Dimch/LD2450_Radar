/**
 * @file LD2450_Radar.h
 * @brief An Arduino library that makes it easy to configure and use
 *           the HiLink 24GHz millimeter-wave Human Presence Detection
 *           sensor (LD2450).
 * @license    The MIT License (MIT)
 */


#ifndef __LD2450_Radar_H__
#define __LD2450_Radar_H__

#include <Arduino.h>
#include <memory>

using namespace std;

typedef struct Region {
  int16_t x1;
  int16_t y1;
  int16_t x2;
  int16_t y2;
} Region_t;

typedef struct ZoneConfig {
  int16_t zoneFilteringMode;
  Region region1;
  Region region2;
  Region region3;
} ZoneConfig_t;

typedef struct Target {
  int16_t x;
  int16_t y;
  int16_t speed;
  uint16_t distanceResolution;
} Target_t;

typedef struct PositionData {
  Target target1;
  Target target2;
  Target target3;
} PositionData_t;

typedef enum BaudRates {
  B_9600 = 0,
  B_19200,
  B_38400,
  B_57600,
  B_115200,
  B_230400,
  B_256000,
  B_460800
} BaudRates_t;

class LD2450_Radar
{
  public:
    /**
      * @brief Constructor
      * @param Stream  Software serial port interface
      */
    LD2450_Radar(HardwareSerial* ser);

    vector<uint8_t> sendCommand(const vector<uint8_t>& commandWord, const vector<uint8_t>& commandValue);
    bool getCommandSuccess(const vector<uint8_t>& response);
    bool beginConfigurationMode();
    bool endConfigurationMode();
    bool setSingleTargetTracking();
    bool setMultiTargetTracking();
    uint16_t getTargetTrackingMode();
    string getFirmwareVersion();
    bool setSerialPortSpeed(BaudRates baudRate = BaudRates::B_256000);
    bool restoreFactorySettings();
    bool restart();
    bool setBluetooth(bool isOn = true);
    string getMacAddress();
    ZoneConfig getZoneConfig();
    bool setZoneConfig(ZoneConfig& zoneConfig);
    PositionData getTargetTrackingData(const vector<uint8_t>& serialData);
    void loop();
 
    /**
     * @brief The serial port (hardware or software) to use for communicating with the sensor
     *
     */
    HardwareSerial* ser;
};

#endif