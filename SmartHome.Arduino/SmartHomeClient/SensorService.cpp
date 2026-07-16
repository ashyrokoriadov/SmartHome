#include <Arduino.h>
#include <SoftwareSerial.h>
#include <string.h>

#include "Config.h"
#include "SensorService.h"
#include "DFRobot_CCS811.h"
#include "DFRobot_BME280.h"
#include "Wire.h"

BME bme(&Wire, 0x76);
DFRobot_CCS811 CCS811;

#define SEA_LEVEL_PRESSURE    1015.0f

bool SensorService::begin()
{
    temperatureSensor.begin();

    pinMode(LIGHT_DIGITAL_PIN, INPUT);

    while(CCS811.begin() != 0){
        Serial.println("failed to init CCS811 chip, please check if the chip connection is fine");
        delay(1000);
    }
    Serial.println("Chip CCS811 was initialized successfully.");

    bme.reset();
    Serial.println("bme read data test");
    while(bme.begin() != BME::eStatusOK) {
        Serial.println("BME initialization failed.");
        printLastOperateStatus(bme.lastOperateStatus);
        delay(2000);
    }
    Serial.println("BME was initialized successfully.");
    delay(100);

    return true;
}

void SensorService::printLastOperateStatus(BME::eStatus_t eStatus)
{
  switch(eStatus) {
    case BME::eStatusOK:    Serial.println("everything ok"); break;
    case BME::eStatusErr:   Serial.println("unknow error"); break;
    case BME::eStatusErrDeviceNotDetected:    Serial.println("device not detected"); break;
    case BME::eStatusErrParameter:    Serial.println("parameter error"); break;
    default: Serial.println("unknow status"); break;
  }
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

AirSensorData SensorService::readAirSensorData()
{
    AirSensorData data;

    if(CCS811.checkDataReady() == true){

        data.CO2 = CCS811.getCO2PPM();
        data.TVOC = CCS811.getTVOCPPB();

        Serial.println(CCS811.readBaseLine(), HEX);
        Serial.print("CO2: ");
        Serial.println(data.CO2);
        Serial.print("ppm, TVOC: ");
        Serial.print(data.TVOC);
        Serial.println("ppb");       
        
    } else {
        Serial.println("Data is not ready!");
    }

    return data;
}

AirParametersData SensorService::readAirParameters()
{
    AirParametersData data;

    data.temperature = bme.getTemperature();
    data.pressure = bme.getPressure();
    data.altitude = bme.calAltitude(SEA_LEVEL_PRESSURE, data.pressure);
    data.humidity = bme.getHumidity();

    return data;
}

SensorData SensorService::readAll()
{
    SensorData data;
    
    AirSensorData airSensorData = readAirSensorData();
    data.CO2 = airSensorData.CO2;
    data.TVOC = airSensorData.TVOC;

    AirParametersData airParametersData = readAirParameters();
    data.temperatureExternal = airParametersData.temperature;
    data.pressure = airParametersData.pressure;
    data.altitude = airParametersData.altitude;
    data.humidity = airParametersData.humidity;

    data.temperature = readTemperature();
    data.lightAnalog = readLightAnalog();
    data.lightDigital = readLightDigital();

    return data;
}
