#include <Arduino.h>
#include "LD2450_Radar.h"

using namespace std;

// tx/rx marks on the sensor connected to following pins on board:
const uint8_t rxPin{20};  // board: 26
const uint8_t txPin{19};  // board: 27

HardwareSerial sensorSerial(1);

// Create a radar instance
LD2450_Radar sensor(&sensorSerial);

void setup() {
  Serial.begin(115200);

  delay(2000);

  sensorSerial.begin(256000, SERIAL_8N1, rxPin, txPin, false, 1000);
  
  Serial.print("Wait for it...");
  delay(2000);
  Serial.println(" OK");

  // Serial.print("Begin configuration mode... ");
  // ok = sensor.enableConfigurationMode();
  // Serial.println(ok ? "Done" : "Failed");
  // if (ok) {
  //   Serial.print("Setting single target tracking mode...");
  //   ok = sensor.setSingleTargetTracking();
  //   Serial.println(ok ? "Done" : "Failed");
  //   Serial.print("End configuration mode...");
  //   ok = sensor.endConfigurationMode();
  //   Serial.println(ok ? "Done" : "Failed");
  // }
  // sensor.beginConfigurationMode();
  sensor.restart();
  Serial.printf("Radar firmware: %s\n", sensor.getFirmwareVersion().c_str());
  // sensor.endConfigurationMode();
  
  // Serial.println("Monitoring presence...");
}

void loop() {
  // NOTE: sending commands is not working again, probable causes:
  //   1. bad connections
  //   2. module firmware version
  //   3. module is not awake?
  sensor.loop();
}
