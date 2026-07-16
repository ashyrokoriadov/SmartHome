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

    float CO2;
    float TVOC;

    float temperatureExternal;
    float pressure;
    float altitude;
    float humidity;
  
    int lightAnalog;
    int lightDigital;
};

struct AirSensorData {
    float CO2;
    float TVOC;
};

struct AirParametersData {
    float temperature;
    float pressure;
    float altitude;
    float humidity;
};

class SensorService {
public:
    bool begin();

    void printLastOperateStatus(BME::eStatus_t eStatus);

    AirSensorData readAirSensorData();
    AirParametersData readAirParameters();

    SensorData readAll();

    float readTemperature();

    int readLightAnalog();
    int readLightDigital();

private:    
    OneWire oneWire{ONE_WIRE_PIN};
    DallasTemperature temperatureSensor{&oneWire};   
};