# SmartHome Arduino Data Collector

This project is a new Arduino C++ application designed for telemetry collection and local Home Assistant MQTT publishing.

## What it does

- Connects to WiFi
- Reads environmental sensors from the existing SmartHome stack
- Reads Victron data for PV voltage and total generated energy
- Publishes one MQTT payload per sensor topic
- Runs a lighting control decision based on time and ambient light
- Keeps the implementation split into multiple independent services following SOLID-style separation of concerns

## Included services

- `App` - application lifecycle and orchestration
- `ClockService` - RTC time management
- `SensorService` - BME280 + ENS160 + temperature/light acquisition
- `LightingService` - daylight and time-based switch logic
- `VictronService` - reading PV data from the Victron serial stream
- `MqttService` - MQTT connectivity and publish operations
- `HomeAssistantPayloadBuilder` - Home Assistant-ready JSON payload generation

## Required Arduino libraries

Install these libraries in the Arduino IDE:

- WiFiS3
- PubSubClient
- RTClib
- OneWire
- DallasTemperature
- DFRobot_BME280
- DFRobot_ENS160
- ArduinoJson

## Configuration

Edit `Config.h` and set:

- `WIFI_SSID`
- `WIFI_PASSWORD`
- `MQTT_BROKER`
- `MQTT_PORT`
- `DEVICE_LOCATION`

## Notes

This version intentionally does not send data to the old API or write to InfluxDB. All sensor values are emitted as Home Assistant-style MQTT messages on dedicated topics.
