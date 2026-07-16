#include <Arduino.h>
#include <SoftwareSerial.h>
#include <string.h>

#include "Config.h"
#include "SensorService.h"
#include "DFRobot_ENS160.h"
#include "DFRobot_BME280.h"
#include "Wire.h"

BME bme(&Wire, 0x76);
DFRobot_ENS160_I2C ENS160(&Wire, 0x53);

#define SEA_LEVEL_PRESSURE    1015.0f

bool SensorService::begin()
{
    temperatureSensor.begin();

    pinMode(LIGHT_DIGITAL_PIN, INPUT);

    int airSensorCounter = 0;

    while( ENS160.begin() != NO_ERR ){
        Serial.println("Communication with device ENS160 failed, please check connection.");
        if (airSensorCounter == 4)
        {
            break;
        }
        delay(1000);    
        airSensorCounter++;    
    }
    Serial.println("Device ENS160 was initialized successfully.");
    ENS160.setPWRMode(ENS160_STANDARD_MODE);

    airSensorCounter = 0;

    bme.reset();
    Serial.println("BME read test data.");
    while(bme.begin() != BME::eStatusOK) {
        Serial.println("BME initialization failed.");
        printLastOperateStatus(bme.lastOperateStatus);
        if (airSensorCounter == 4)
        {
            break;
        }
        delay(2000);
        airSensorCounter++;    
    }
    Serial.println("BME was initialized successfully.");
    delay(100);
    airSensorCounter = 0;

    ENS160.setTempAndHum(bme.getTemperature(), bme.getHumidity());

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

AirPolutionData SensorService::readAirPolutionData()
{
    AirPolutionData data;

    uint8_t Status = ENS160.getENS160Status();
    Serial.print("Sensor operating status : ");
    Serial.println(Status);

    uint8_t AQI = ENS160.getAQI();
    Serial.print("Air quality index : ");
    Serial.println(AQI);

    uint16_t TVOC = ENS160.getTVOC();
    Serial.print("Concentration of total volatile organic compounds : ");
    Serial.print(TVOC);
    Serial.println(" ppb");

    uint16_t ECO2 = ENS160.getECO2();
    Serial.print("Carbon dioxide equivalent concentration : ");
    Serial.print(ECO2);
    Serial.println(" ppm");
    Serial.println();
    
    data.AQI = AQI;
    data.TVOC = TVOC;
    data.ECO2 = ECO2;

    return data;
}

AirQualityData SensorService::readAirQualityData()
{
    AirQualityData data;

    data.temperature = bme.getTemperature();
    data.pressure = bme.getPressure();
    data.altitude = bme.calAltitude(SEA_LEVEL_PRESSURE, data.pressure);
    data.humidity = bme.getHumidity();

    return data;
}

SensorData SensorService::readAll()
{
    SensorData data;
    
    AirPolutionData airPolutionData = readAirPolutionData();
    data.ECO2 = airPolutionData.ECO2;
    data.TVOC = airPolutionData.TVOC;
    data.AQI = airPolutionData.AQI;

    AirQualityData airQualityData = readAirQualityData();
    data.temperatureExternal = airQualityData.temperature;
    data.pressure = airQualityData.pressure;
    data.altitude = airQualityData.altitude;
    data.humidity = airQualityData.humidity;

    data.temperature = readTemperature();
    data.lightAnalog = readLightAnalog();
    data.lightDigital = readLightDigital();

    return data;
}
