#pragma once

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "DFRobot_BME280.h"
#include "DFRobot_ENS160.h"
#include "Config.h"

typedef DFRobot_BME280_IIC BME;
extern BME bme;

struct AirQualityData {
    float temperature;
    float pressure;
    float altitude;
    float humidity;
};

struct AirPollutionData {
    uint8_t status;
    uint16_t AQI;
    uint16_t TVOC;
    uint16_t ECO2;
};

struct SensorData {
    float temperature;
    float temperatureExternal;
    float pressure;
    float altitude;
    float humidity;
    uint8_t airStatus;
    uint16_t airQualityIndex;
    uint16_t tvoc;
    uint16_t eco2;
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
    AirQualityData readAirQuality();
    AirPollutionData readAirPollution();

private:
    OneWire oneWire{ONE_WIRE_PIN};
    DallasTemperature temperatureSensor{&oneWire};
};
