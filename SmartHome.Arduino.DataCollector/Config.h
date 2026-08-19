#pragma once

#include <Arduino.h>

constexpr char WIFI_SSID[] = "YOUR_WIFI_SSID";
constexpr char WIFI_PASSWORD[] = "YOUR_WIFI_PASSWORD";
constexpr char MQTT_BROKER[] = "192.168.1.94";
constexpr uint16_t MQTT_PORT = 1883;
constexpr char MQTT_BASE_TOPIC[] = "smarthome";
constexpr char DEVICE_LOCATION[] = "Balcony";

constexpr uint8_t ONE_WIRE_PIN = 4;
constexpr uint8_t LIGHT_ANALOG_PIN = A0;
constexpr uint8_t LIGHT_DIGITAL_PIN = 2;
constexpr uint8_t LAMPS_CONTROL_PIN = 7;
constexpr uint8_t VICTRON_RX_PIN = 0;
constexpr uint8_t VICTRON_TX_PIN = 1;

constexpr unsigned long SENSOR_INTERVAL_MS = 10UL * 1000UL;
constexpr unsigned long VICRON_INTERVAL_MS = 10UL * 1000UL;
constexpr unsigned long WIFI_RETRY_MS = 5UL * 1000UL;
constexpr unsigned long MQTT_RETRY_MS = 10UL * 1000UL;

constexpr float SEA_LEVEL_PRESSURE_HPA = 1015.0f;
