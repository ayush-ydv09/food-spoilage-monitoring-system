#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>

// Configure these locally. Never commit real Wi-Fi credentials.
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

const char* MQTT_SERVER = "broker.hivemq.com";
const uint16_t MQTT_PORT = 1883;
const char* DEVICE_ID = "ayush-7k3q-esp32";
const char* TELEMETRY_TOPIC = "foodmonitor/ayush-7k3q-esp32/telemetry";
const char* STATUS_TOPIC = "foodmonitor/ayush-7k3q-esp32/status";

#define DHT_PIN 4
#define DHT_TYPE DHT22
#define MQ135_PIN 35
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_SDA 18
#define OLED_SCL 19
#define OLED_ADDRESS 0x3C

DHT dht(DHT_PIN, DHT_TYPE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

bool oledReady = false;
float temperatureC = NAN;
float humidityPct = NAN;
int mq135Raw = 0;
unsigned long lastSensorRead = 0;
unsigned long lastMqttRetry = 0;
const unsigned long SENSOR_INTERVAL_MS = 5000;
const unsigned long MQTT_RETRY_MS = 5000;

void initializeDisplay() {
  Wire.begin(OLED_SDA, OLED_SCL);
  Wire.setClock(100000);
  oledReady = display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS);
  if (!oledReady) {
    Serial.println("OLED initialization failed; continuing without display.");
    return;
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 12);
  display.println("FOOD MONITOR");
  display.setCursor(0, 28);
  display.println("Starting...");
  display.display();
}

void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;

  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  const unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi connected.");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("RSSI: ");
    Serial.println(WiFi.RSSI());
  } else {
    Serial.println("\nWi-Fi connection failed; will retry.");
  }
}

void connectMqtt() {
  if (WiFi.status() != WL_CONNECTED || mqttClient.connected()) return;

  Serial.print("Connecting to MQTT broker...");
  const bool connected = mqttClient.connect(
    DEVICE_ID, STATUS_TOPIC, 0, true, "offline"
  );

  if (connected) {
    Serial.println("connected");
    mqttClient.publish(STATUS_TOPIC, "online", true);
  } else {
    Serial.print("failed, state=");
    Serial.println(mqttClient.state());
  }
}

void readSensors() {
  temperatureC = dht.readTemperature();
  humidityPct = dht.readHumidity();
  mq135Raw = analogRead(MQ135_PIN);

  Serial.println("\n--- Sensor readings ---");
  if (isnan(temperatureC)) Serial.println("Temperature: invalid");
  else {
    Serial.print("Temperature: ");
    Serial.print(temperatureC, 2);
    Serial.println(" C");
  }

  if (isnan(humidityPct)) Serial.println("Humidity: invalid");
  else {
    Serial.print("Humidity: ");
    Serial.print(humidityPct, 2);
    Serial.println(" %");
  }

  Serial.print("MQ-135 raw ADC: ");
  Serial.println(mq135Raw);
}

void updateDisplay() {
  if (!oledReady) return;

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("FOOD MONITOR");
  display.drawLine(0, 11, 127, 11, SSD1306_WHITE);

  display.setCursor(0, 17);
  display.print("Temp: ");
  if (isnan(temperatureC)) display.println("--");
  else {
    display.print(temperatureC, 1);
    display.println(" C");
  }

  display.setCursor(0, 29);
  display.print("Humidity: ");
  if (isnan(humidityPct)) display.println("--");
  else {
    display.print(humidityPct, 1);
    display.println(" %");
  }

  display.setCursor(0, 41);
  display.print("MQ135 ADC: ");
  display.println(mq135Raw);

  display.setCursor(0, 53);
  display.print("MQTT: ");
  display.println(mqttClient.connected() ? "CONNECTED" : "OFFLINE");
  display.display();
}

void publishTelemetry() {
  if (!mqttClient.connected()) return;

  char tempText[16];
  char humidityText[16];
  if (isnan(temperatureC)) strcpy(tempText, "null");
  else snprintf(tempText, sizeof(tempText), "%.2f", temperatureC);

  if (isnan(humidityPct)) strcpy(humidityText, "null");
  else snprintf(humidityText, sizeof(humidityText), "%.2f", humidityPct);

  char payload[256];
  snprintf(
    payload, sizeof(payload),
    "{\"device_id\":\"%s\",\"temperature\":%s,\"humidity\":%s,"
    "\"mq135_raw\":%d,\"rssi\":%d,\"status\":\"online\"}",
    DEVICE_ID, tempText, humidityText, mq135Raw, WiFi.RSSI()
  );

  if (mqttClient.publish(TELEMETRY_TOPIC, payload)) {
    Serial.print("Published: ");
    Serial.println(payload);
  } else {
    Serial.println("MQTT publish failed.");
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);
  analogReadResolution(12);
  dht.begin();
  initializeDisplay();

  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setKeepAlive(30);
  mqttClient.setSocketTimeout(5);
  mqttClient.setBufferSize(512);

  connectWiFi();
  connectMqtt();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (WiFi.status() == WL_CONNECTED) {
    if (!mqttClient.connected() &&
        millis() - lastMqttRetry >= MQTT_RETRY_MS) {
      lastMqttRetry = millis();
      connectMqtt();
    }
    mqttClient.loop();
  }

  if (millis() - lastSensorRead >= SENSOR_INTERVAL_MS) {
    lastSensorRead = millis();
    readSensors();
    updateDisplay();
    publishTelemetry();
  }
}
