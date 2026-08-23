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

bool SensorService::recoverI2CBus()
{
    Serial.println("I2C recovery started");

    pinMode(SDA, INPUT);
    pinMode(SCL, INPUT);

    delay(5);

    bool sda = digitalRead(SDA);
    bool scl = digitalRead(SCL);

    Serial.print("I2C before recovery: SDA=");
    Serial.print(sda);
    Serial.print(" SCL=");
    Serial.println(scl);

    // SCL LOW -> There is no a good way to recover SCL LOW oznacza.
    if (scl == LOW)
    {
        Serial.println("I2C recovery failed: SCL is LOW");
        return false;
    }

    // SDA LOW -> try to push a slave out of never ending trasfer.
    if (sda == LOW)
    {
        Serial.println("I2C recovery: SDA is LOW");

        pinMode(SCL, OUTPUT);

        for (int i = 0; i < 9; ++i)
        {
            digitalWrite(SCL, LOW);
            delayMicroseconds(10);

            digitalWrite(SCL, HIGH);
            delayMicroseconds(10);

            if (digitalRead(SDA) == HIGH)
            {
                Serial.print("I2C recovery: SDA released after ");
                Serial.print(i + 1);
                Serial.println(" clocks");
                break;
            }
        }

        pinMode(SCL, INPUT);
    }

    if (digitalRead(SDA) == LOW || digitalRead(SCL) == LOW)
    {
        Serial.print("I2C recovery failed: SDA=");
        Serial.print(digitalRead(SDA));
        Serial.print(" SCL=");
        Serial.println(digitalRead(SCL));

        return false;
    }

    Serial.println("I2C bus released");

    Wire.end();
    delay(5);

    Wire.begin();
    delay(5);

    Serial.println("I2C scan after recovery:");

    int devicesFound = 0;

    for (uint8_t address = 1; address < 127; ++address)
    {
        Wire.beginTransmission(address);
        uint8_t error = Wire.endTransmission();

        if (error == 0)
        {
            Serial.print("  0x");

            if (address < 16)
                Serial.print("0");

            Serial.print(address, HEX);
            Serial.println(" ACK");

            devicesFound++;
        }
    }

    if (devicesFound == 0)
    {
        Serial.println("  No I2C devices found!");
    }
    else
    {
        Serial.print("I2C scan: ");
        Serial.print(devicesFound);
        Serial.println(" device(s) found");
    }

    this->begin();

    Serial.println("I2C Wire restarted");

    return true;
}
