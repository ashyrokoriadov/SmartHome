#pragma once

#include <Arduino.h>
#include "Config.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_INA219.h>

struct SensorData {
    float temperature;
    float voltage;
    float current;

    int lightAnalog;
    bool lightDigital;
};

class SensorService {
public:
    bool begin();

    SensorData readAll();

    float readTemperature();
    float readVoltage();
    float readCurrent();

    int readLightAnalog();
    bool readLightDigital();

private:
    OneWire oneWire{ONE_WIRE_PIN};
    DallasTemperature temperatureSensor{&oneWire};

    Adafruit_INA219 ina219;
};