#include <Arduino.h>
#include <SoftwareSerial.h>
#include <string.h>

#include "Config.h"
#include "SensorService.h"

bool SensorService::begin()
{
    temperatureSensor.begin();

    pinMode(LIGHT_DIGITAL_PIN, INPUT);

    return true;
}

float SensorService::readTemperature()
{
    temperatureSensor.requestTemperatures();

    return temperatureSensor.getTempCByIndex(0);
}

int SensorService::readLightAnalog()
{
    return analogRead(LIGHT_ANALOG_PIN);
}

int SensorService::readLightDigital()
{
    return digitalRead(LIGHT_DIGITAL_PIN);
}

SensorData SensorService::readAll()
{
    SensorData data;

    data.lightAnalog = readLightAnalog();
    data.lightDigital = readLightDigital();

    return data;
}
