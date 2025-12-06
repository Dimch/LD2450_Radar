# LD2450 Radar Arduino Library

A lightweight Arduino-friendly wrapper around the HiLink LD2450 24GHz human presence radar. The library hides the byte-level protocol so you can configure the module, read target positions, and prototype interactive projects on ESP32-S3 hardware.

## Features
- Configuration helpers for single or multi-target tracking
- Zone filtering read/write helpers for three rectangular regions
- Human-friendly accessors for firmware, MAC address, and module state
- Streaming loop helper that continuously parses UART reports into up to three tracked targets
- PlatformIO project pre-configured for ESP32-S3 WROOM modules using the Arduino framework

## Hardware Setup
1. Wire the radar module's `TX` pin to ESP32 `GPIO20` (library constant `RADAR_RX_PIN`).
2. Wire the radar `RX` pin to ESP32 `GPIO19` (constant `RADAR_TX_PIN`).
3. Provide 5V (or module-rated) power and GND shared between the radar and the ESP32 board.
4. Connect the board to your computer and confirm it enumerates a serial port.

Adjust the pin constants in `examples/main.cpp` if your board uses different routing.

## Getting Started (PlatformIO)
1. Clone the repository and open it in VS Code with the PlatformIO extension.
2. Make sure you have the Espressif32 toolchain installed (`pio platform install espressif32`).
3. Build the default environment:
   ```sh
   pio run
   ```
4. Upload to the connected board:
   ```sh
   pio run -t upload
   ```
5. Open the serial monitor at 115200 baud to watch radar readings:
   ```sh
   pio device monitor
   ```

The default environment (`esp32-s3-wroom1-n4r2`) is defined in `platformio.ini`. Adjust it if you target a different board.

## Using This Library in Other Projects
Until the package is published to the PlatformIO registry you can pull it straight from GitHub:

```ini
; platformio.ini
[env:your-board]
platform = espressif32
framework = arduino
lib_deps =
   https://github.com/Dimch/LD2450_Radar.git
```

Once published, replace the Git URL with the registry identifier (for example `Dimch/LD2450_Radar@^0.1.0`).

## Publishing to the PlatformIO Registry
1. Log in to PlatformIO: `pio account login`.
2. From the repository root, run `pio pkg publish` (add `--owner <org>` if needed).
3. Tag new releases (`git tag v0.1.0 && git push origin v0.1.0`) so PlatformIO users can depend on semver ranges.
4. After each new tag, rerun `pio pkg publish` to update the registry entry.

## Example Usage
`examples/main.cpp` demonstrates the typical flow: initialize the UART, restart the module, optionally switch tracking modes, and call `radar.loop()` to stream parsed targets.

```cpp
#include <Arduino.h>
#include "LD2450_Radar.h"

constexpr uint8_t RADAR_RX_PIN = 20;
constexpr uint8_t RADAR_TX_PIN = 19;
constexpr uint32_t RADAR_BAUD = 256000;

HardwareSerial radarSerial(1);
LD2450_Radar radar(&radarSerial);

void setup() {
  Serial.begin(115200);
  radarSerial.begin(RADAR_BAUD, SERIAL_8N1, RADAR_RX_PIN, RADAR_TX_PIN, false, 1000);
  radar.restart();
  radar.beginConfigurationMode();
  radar.setMultiTargetTracking();
  radar.endConfigurationMode();
}

void loop() {
  radar.loop();
  delay(50);
}
```

The `loop()` helper consumes radar UART frames and prints each target (x/y in millimeters, speed in cm/s, and distance resolution).

## API Highlights
- `sendCommand()` / `getCommandSuccess()` for low-level protocol access
- `beginConfigurationMode()` / `endConfigurationMode()` to safely modify settings
- `setSingleTargetTracking()` and `setMultiTargetTracking()` switch tracking modes
- `getFirmwareVersion()`, `setSerialPortSpeed()`, `restoreFactorySettings()`, `restart()` manage module state
- `getZoneConfig()` / `setZoneConfig()` read or write the three-zone filter
- `loop()` + `getTargetTrackingData()` translate raw UART frames into three `Target_t` structs

See `include/LD2450_Radar.h` for the full public surface.

## Testing
The repository includes GoogleTest-based unit tests targeting an onboard environment. Run them with:
```sh
pio test -e test-onboard
```

## License
Released under the MIT License. See `LICENSE` for details.

## AI-Generated Documentation Disclaimer
Portions of this documentation were generated with the assistance of AI tooling. Please review and validate the information against the source code and official LD2450 documentation before use in production systems.
