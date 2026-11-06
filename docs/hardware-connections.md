# Hardware connections

## ESP32 pin map

| Component | Signal | ESP32 |
|---|---|---|
| DHT22 | DATA | GPIO 4 |
| MQ-135 | Analogue output | GPIO 35 through a voltage divider |
| SSD1306 OLED | SDA | GPIO 18 |
| SSD1306 OLED | SCL | GPIO 19 |
| SSD1306 OLED | I²C address | 0x3C |

## Power and electrical precautions

- Confirm the exact supply requirements of each module before wiring.
- The MQ-135 module in this prototype is powered from the 5 V/VIN rail.
- ESP32 GPIO/ADC pins are not 5 V tolerant. Scale the MQ-135 analogue output with a properly calculated divider and verify the maximum voltage at GPIO35.
- Ensure all modules share a common ground.
- Avoid powering a high-current MQ sensor from a weak 3.3 V regulator.
- Disconnect power before changing wiring.

## Firmware constants

The firmware currently defines DHT22 on GPIO4, MQ-135 ADC on GPIO35, OLED SDA/SCL on GPIO18/GPIO19 and OLED address 0x3C. Confirm these against the physical board before uploading.
