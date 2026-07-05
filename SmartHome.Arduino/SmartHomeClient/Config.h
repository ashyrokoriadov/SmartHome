#pragma once

#include <Arduino.h>

constexpr char API_HOST[] = "192.168.1.94";
constexpr uint16_t API_PORT = 5000;

constexpr uint8_t ONE_WIRE_PIN = 4;
constexpr uint8_t LIGHT_ANALOG_PIN = A0;
constexpr uint8_t LIGHT_DIGITAL_PIN = 2;
constexpr uint8_t LAMPS_CONTROL_PIN = 7;
constexpr uint8_t VICTRON_RX_PIN = 8;
constexpr uint8_t VICTRON_TX_PIN = 9;

// WiFi credentials (fill in your network SSID and password)
constexpr char WIFI_SSID[] = "SECRET";
constexpr char WIFI_PASSWORD[] = "SECRET";

// Default device location
constexpr char LOCATION[] = "Balcony";

// Matrix display configuration
constexpr uint32_t MATRIX_STROKE_COLOR = 0xFFFFFF;
constexpr int MATRIX_TEXT_SCROLL_SPEED = 160;
#define MATRIX_TEXT_FONT Font_5x7
constexpr int MATRIX_TEXT_Y = 1;
constexpr int MATRIX_WIDTH = 12; // used for centering static text

//const unsigned long API_REQUEST_INTERVAL = 2UL * 60UL * 1000UL; // 2 minutes
const unsigned long API_REQUEST_INTERVAL = 15UL * 1000UL; // 15 seconds
const unsigned long API_REQUEST_FAIL_WAIT_TIME = 15UL * 60UL * 1000UL; // 15 minutes