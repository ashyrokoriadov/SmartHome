#pragma once

#include <Arduino.h>
#include "Config.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>
#include <string.h>
#include "DFRobot_CCS811.h"
#include "DFRobot_BME280.h"

typedef DFRobot_BME280_IIC BME;
extern BME bme;   

struct SensorData {
    float temperature;

    float ECO2;
    float TVOC;
    float AQI;
    int status;

    float temperatureExternal;
    float pressure;
    float altitude;
    float humidity;
  
    int lightAnalog;
    int lightDigital;
};

struct AirPolutionData {
    float ECO2;
    float TVOC;
    float AQI;
    int status;
};

struct AirQualityData {
    float temperature;
    float pressure;
    float altitude;
    float humidity;
};

class SensorService {
public:
    bool begin();

    void printLastOperateStatus(BME::eStatus_t eStatus);

    AirPolutionData readAirPolutionData();
    AirQualityData readAirQualityData();

    SensorData readAll();

    float readTemperature();

    int readLightAnalog();
    int readLightDigital();

private:    
    OneWire oneWire{ONE_WIRE_PIN};
    DallasTemperature temperatureSensor{&oneWire};   
};