#include <Arduino.h>

#include "Config.h"
#include "SensorService.h"

bool SensorService::begin()
{
    temperatureSensor.begin();

    pinMode(LIGHT_DIGITAL_PIN, INPUT);

    if (!ina219.begin())
    {
        Serial.println("INA219 not found");
        return false;
    }

    return true;
}

float SensorService::readTemperature()
{
    temperatureSensor.requestTemperatures();

    return temperatureSensor.getTempCByIndex(0);
}

float SensorService::readVoltage()
{
    return ina219.getBusVoltage_V();
}

float SensorService::readCurrent()
{
    return ina219.getCurrent_mA() / 1000.0f;
}

int SensorService::readLightAnalog()
{
    return analogRead(LIGHT_ANALOG_PIN);
}

bool SensorService::readLightDigital()
{
    return digitalRead(LIGHT_DIGITAL_PIN);
}

SensorData SensorService::readAll()
{
    SensorData data;

    data.temperature = readTemperature();
    data.voltage = readVoltage();
    data.current = readCurrent();

    data.lightAnalog = readLightAnalog();
    data.lightDigital = readLightDigital();

    return data;
}
