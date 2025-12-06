#include <Arduino.h>
#include "LD2450_Radar.h"

constexpr uint8_t RADAR_RX_PIN = 20;   // sensor TX pin
constexpr uint8_t RADAR_TX_PIN = 19;   // sensor RX pin
constexpr uint32_t RADAR_BAUD = 256000;

HardwareSerial radarSerial(1);
LD2450_Radar radar(&radarSerial);

bool configureTrackingMode() {
  Serial.print("Entering configuration mode... ");
  if (!radar.beginConfigurationMode()) {
    Serial.println("failed");
    return false;
  }
  Serial.println("ok");

  Serial.print("Switching to multi-target tracking... ");
  const bool trackingOk = radar.setMultiTargetTracking();
  Serial.println(trackingOk ? "ok" : "failed");

  Serial.print("Leaving configuration mode... ");
  const bool exitOk = radar.endConfigurationMode();
  Serial.println(exitOk ? "ok" : "failed");

  return trackingOk && exitOk;
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 2000) {
    delay(10);
  }

  Serial.println();
  Serial.println("LD2450 radar quickstart");
  Serial.println("Open the Serial Monitor @115200 to see target updates.");

  radarSerial.begin(RADAR_BAUD, SERIAL_8N1, RADAR_RX_PIN, RADAR_TX_PIN, false, 1000);
  delay(200);  // give the radar time to boot

  Serial.print("Restarting radar module... ");
  Serial.println(radar.restart() ? "ok" : "failed (continuing anyway)");

  const auto firmware = radar.getFirmwareVersion();
  if (!firmware.empty()) {
    Serial.printf("Firmware version: %s\n", firmware.c_str());
  }

  if (configureTrackingMode()) {
    Serial.println("Multi-target tracking ready (up to three targets).");
  } else {
    Serial.println("Using the module's default tracking mode.");
  }

  Serial.println("Move in front of the radar to see x/y/speed readings.");
}

void loop() {
  radar.loop();
  delay(50);  // throttle output so it stays readable
}
