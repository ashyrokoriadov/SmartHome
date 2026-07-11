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
    
    OneWire oneWire{ONE_WIRE_PIN};
    DallasTemperature temperatureSensor{&oneWire};   

    //Adafruit_INA219 ina219;
};