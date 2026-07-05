#pragma once

#include <Arduino.h>
#include "Config.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>
#include <string.h>
//#include <Adafruit_INA219.h>

struct SensorData {
    float temperature;
    float voltage;
    float current;
    float panelVoltage;
    float panelPower;
    int chargerState;
    int error;
    float gainedEnergyToday;

    int lightAnalog;
    int lightDigital;
};

class SensorService {
public:
    bool begin();

    SensorData readAll();

    float readTemperature();

    int readLightAnalog();
    int readLightDigital();

private:
    void readVictronTelemetry();
    void parseVictronLine(const char* line);
    float parseVictronValue(const char* valueText, bool divideBy1000) const;

    OneWire oneWire{ONE_WIRE_PIN};
    DallasTemperature temperatureSensor{&oneWire};
    SoftwareSerial victronSerial{VICTRON_RX_PIN, VICTRON_TX_PIN};

    float lastVoltage = 0.0f;
    float lastCurrent = 0.0f;
    float lastPanelVoltage = 0.0f;
    float lastPanelPower = 0.0f;
    int lastChargerState = 0;
    int lastError = 0;
    float lastGainedEnergyToday = 0.0f;

    //Adafruit_INA219 ina219;
};