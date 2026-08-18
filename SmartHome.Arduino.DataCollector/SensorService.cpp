#include "SensorService.h"

BME bme(&Wire, 0x76);
DFRobot_ENS160_I2C ens160(&Wire, 0x53);

bool SensorService::begin()
{
    temperatureSensor.begin();
    pinMode(LIGHT_DIGITAL_PIN, INPUT);

    int retries = 0;
    while (ens160.begin() != NO_ERR) {
        Serial.println("ENS160 init failed.");
        if (retries++ >= 4) {
            break;
        }
        delay(1000);
    }

    ens160.setPWRMode(ENS160_STANDARD_MODE);

    retries = 0;
    bme.reset();
    while (bme.begin() != BME::eStatusOK) {
        Serial.println("BME280 init failed.");
        if (retries++ >= 4) {
            break;
        }
        delay(2000);
    }

    ens160.setTempAndHum(bme.getTemperature(), bme.getHumidity());
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

AirQualityData SensorService::readAirQuality()
{
    AirQualityData data;
    data.temperature = bme.getTemperature();
    data.pressure = bme.getPressure() / 100.0f;
    data.altitude = bme.calAltitude(SEA_LEVEL_PRESSURE_HPA, data.pressure);
    data.humidity = bme.getHumidity();
    return data;
}

AirPollutionData SensorService::readAirPollution()
{
    AirPollutionData data;
    data.status = ens160.getENS160Status();
    data.AQI = ens160.getAQI();
    data.TVOC = ens160.getTVOC();
    data.ECO2 = ens160.getECO2();
    return data;
}

SensorData SensorService::readAll()
{
    SensorData data{};

    AirPollutionData airPollution = readAirPollution();
    AirQualityData airQuality = readAirQuality();

    data.temperature = readTemperature();
    data.temperatureExternal = airQuality.temperature;
    data.pressure = airQuality.pressure;
    data.altitude = airQuality.altitude;
    data.humidity = airQuality.humidity;
    data.airStatus = airPollution.status;
    data.airQualityIndex = airPollution.AQI;
    data.tvoc = airPollution.TVOC;
    data.eco2 = airPollution.ECO2;
    data.lightAnalog = readLightAnalog();
    data.lightDigital = readLightDigital();

    return data;
}
