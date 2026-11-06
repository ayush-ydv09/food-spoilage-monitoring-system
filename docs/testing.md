# Bring-up and experimental testing

## 1. Hardware bring-up

- [ ] Verify ESP32 board selection and USB serial port.
- [ ] Confirm DHT22 wiring and check for valid temperature/humidity values.
- [ ] Scan the I²C bus and confirm the OLED responds at 0x3C.
- [ ] Confirm the OLED displays sensor values.
- [ ] Verify the MQ-135 ADC signal is within the ESP32 input-voltage limit.
- [ ] Record the raw ADC range after sensor warm-up.

## 2. Network and MQTT

- [ ] Configure Wi-Fi credentials locally; never commit them.
- [ ] Confirm the ESP32 joins a 2.4 GHz Wi-Fi network.
- [ ] Confirm the MQTT broker and port are reachable.
- [ ] Subscribe to the telemetry topic with an MQTT client.
- [ ] Check that the JSON payload arrives at the expected interval.
- [ ] Confirm the browser dashboard uses the same broker and topic names.
- [ ] Test reconnect behaviour after briefly disabling Wi-Fi or broker access.

## 3. Sensor characterization

MQ-135 readings are influenced by warm-up history, humidity, temperature, sensor ageing and multiple gases. A raw ADC number is not a selective measurement of food spoilage.

For each experiment, record:
- sample identity and independent reference label;
- sensor warm-up time and elapsed time;
- temperature and relative humidity;
- MQ-135 raw ADC value;
- replicate number and measurement interval;
- environmental conditions and any handling differences.

Use repeated trials and a documented baseline. Do not set universal spoilage thresholds from a single run. Any future classifier requires a sufficiently large, independently labelled dataset and validation on samples not used for threshold selection or training.

## 4. Reporting

Report the prototype as an environmental/gas-response monitoring system until the sensing method and classification performance have been validated. Include limitations and avoid food-safety claims.
