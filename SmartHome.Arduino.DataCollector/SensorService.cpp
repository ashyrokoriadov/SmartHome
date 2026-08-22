#include "SensorService.h"

BME bme(&Wire, 0x76);
DFRobot_ENS160_I2C ens160(&Wire, 0x53);

bool SensorService::begin()
{
    temperatureSensor.begin();
    pinMode(LIGHT_DIGITAL_PIN, INPUT);

    int retries = 0;
    bool ens160Ok = false;

    while (retries++ < 5)
    {
        if (ens160.begin() == NO_ERR)
        {
            ens160Ok = true;
            break;
        }

        Serial.println("ENS160 init failed.");
        delay(1000);
    }

    if (ens160Ok)
        ens160.setPWRMode(ENS160_STANDARD_MODE);

    retries = 0;
    bool bmeOk = false;

    bme.reset();

    while (retries++ < 5)
    {
        if (bme.begin() == BME::eStatusOK)
        {
            bmeOk = true;
            break;
        }

        Serial.println("BME280 init failed.");
        delay(2000);
    }

    if (ens160Ok && bmeOk)
    {
        ens160.setTempAndHum(
            bme.getTemperature(),
            bme.getHumidity()
        );
    }

    return ens160Ok && bmeOk;
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

    // Log all sensor values as key=value pairs for diagnostics
    {
        String log = "{";
        log += "temperature=" + String(data.temperature) + ", ";
        log += "temperatureExternal=" + String(data.temperatureExternal) + ", ";
        log += "pressure=" + String(data.pressure) + ", ";
        log += "altitude=" + String(data.altitude) + ", ";
        log += "humidity=" + String(data.humidity) + ", ";
        log += "airStatus=" + String(data.airStatus) + ", ";
        log += "airQualityIndex=" + String(data.airQualityIndex) + ", ";
        log += "tvoc=" + String(data.tvoc) + ", ";
        log += "eco2=" + String(data.eco2) + ", ";
        log += "lightAnalog=" + String(data.lightAnalog) + ", ";
        log += "lightDigital=" + String(data.lightDigital);
        log += "}";
        Serial.println(log);
    }

    return data;
}
