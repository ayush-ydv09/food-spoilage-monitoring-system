# IoT-Based Food Spoilage Monitoring System

An ESP32-based experimental monitoring prototype that streams temperature, relative humidity and MQ-135 sensor readings over MQTT to a browser dashboard.

> **Research note:** This is a sensor-monitoring prototype, not a validated food-safety instrument. MQ-135 raw ADC values are affected by warm-up, humidity, temperature, sensor ageing and cross-sensitivity. The dashboard's indicators are experimental and must not be interpreted as a definitive spoilage or food-safety verdict.

## System overview

```text
DHT22 ─────┐
           ├── ESP32 ── Wi-Fi ── MQTT broker ── Web dashboard
MQ-135 ────┘      │
                  └── SSD1306 OLED
```

The ESP32 samples the sensors, updates the OLED and publishes JSON telemetry. The browser dashboard subscribes to the device's telemetry and status topics.

## Hardware

- ESP32 development board (ESP32-WROOM)
- DHT22 temperature and humidity sensor
- MQ-135 gas sensor module
- 0.96-inch SSD1306 I²C OLED (128 × 64, address `0x3C`)
- Breadboard, jumper wires and suitable power supply

## Pin mapping

| Module | ESP32 connection |
|---|---|
| DHT22 DATA | GPIO 4 |
| MQ-135 AO | GPIO 35 through a suitable voltage divider |
| OLED SDA | GPIO 18 |
| OLED SCL | GPIO 19 |
| OLED address | `0x3C` |

The MQ module is powered from the board's 5 V/VIN supply in this build. **Do not connect a 5 V analogue output directly to an ESP32 ADC pin.** Verify the module's output range and use a correctly calculated divider so the ESP32 input remains within its permitted voltage.

## Repository layout

- `firmware/food_spoilage_mqtt/` — ESP32 firmware for sensor acquisition, OLED output and MQTT publishing.
- `dashboard/` — browser dashboard for live MQTT telemetry.
- `docs/hardware-connections.md` — wiring and electrical notes.
- `docs/testing.md` — bring-up and characterization checklist.

## Firmware setup

1. Install Arduino IDE and the ESP32 board support package.
2. Install **PubSubClient**, **DHT sensor library (Adafruit)**, **Adafruit GFX Library**, **Adafruit SSD1306**, and **Adafruit Unified Sensor**.
3. Open `firmware/food_spoilage_mqtt/food_spoilage_mqtt.ino`.
4. Replace `YOUR_WIFI_SSID` and `YOUR_WIFI_PASSWORD` with local 2.4 GHz Wi-Fi credentials. Do not commit real credentials.
5. Confirm the MQTT broker, port, device ID and topic names match the dashboard configuration.
6. Select the correct ESP32 board and port, upload, then inspect Serial Monitor at 115200 baud.

## Dashboard setup

Open `dashboard/index.html` in a modern browser. It connects to the public HiveMQ WebSocket endpoint and subscribes to the configured topics. For a reliable browser connection, serve the file from localhost or an HTTPS static host.

The public broker is suitable only for demonstration. Public topics are not private: use a private broker with authentication and TLS for real deployments, and choose a unique topic namespace.

## MQTT contract

- Telemetry: `foodmonitor/ayush-7k3q-esp32/telemetry`
- Status: `foodmonitor/ayush-7k3q-esp32/status`
- Publish interval: approximately 5 seconds
- Telemetry fields: device ID, temperature, humidity, MQ-135 raw ADC, Wi-Fi RSSI and status

The MQ-135 value is an **ADC reading**, not a calibrated gas concentration (ppm).

## Experimental validation plan

1. Allow the MQ-135 sensor to warm up according to its datasheet and record the warm-up period.
2. Capture repeated baseline readings under controlled, documented conditions.
3. Record temperature and humidity alongside every gas-sensor reading.
4. Repeat measurements across multiple sessions and compare variability.
5. Test against independently labelled reference samples before making any classification claim.
6. Report sensor limitations, sample count, conditions and uncertainty.

## Current scope and limitations

- Measures environmental conditions and a gas-sensor response.
- Sends telemetry over Wi-Fi/MQTT and displays it in a browser.
- Does not currently establish a scientifically validated food-spoilage classifier.
- No food should be declared safe or unsafe based on this prototype.

## License

No license has been added yet. Until a license is selected, the default copyright rules apply.
