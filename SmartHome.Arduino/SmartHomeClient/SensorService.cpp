#include <Arduino.h>
#include <SoftwareSerial.h>

#include "Config.h"
#include "SensorService.h"

bool SensorService::begin()
{
    temperatureSensor.begin();

    pinMode(LIGHT_DIGITAL_PIN, INPUT);
    victronSerial.begin(19200);

#if defined(ARDUINO_ARCH_AVR)
    victronSerial.listen();
#endif

    return true;
}

void SensorService::readVictronTelemetry()
{
    static bool hasLoggedVictronData = false;
    static unsigned long lastVictronLogMillis = 0;
    static unsigned long lastVictronActivityMillis = 0;

    const unsigned long victronLogInterval = 2000UL;
    const unsigned long victronIdleLogInterval = 10000UL;

    String line;
    bool receivedData = false;

    while (victronSerial.available())
    {
        receivedData = true;
        lastVictronActivityMillis = millis();

        char c = static_cast<char>(victronSerial.read());

        if (c == '\n' || c == '\r')
        {
            if (line.length() > 0)
            {
                parseVictronLine(line);
                line = "";
            }
        }
        else
        {
            line += c;
        }
    }

    if (receivedData)
    {
        const unsigned long now = millis();
        if (!hasLoggedVictronData || (now - lastVictronLogMillis >= victronLogInterval))
        {
            Serial.println(F("Victron telemetry received."));
            lastVictronLogMillis = now;
            hasLoggedVictronData = true;
        }
    }
    else if (hasLoggedVictronData && (millis() - lastVictronActivityMillis >= victronIdleLogInterval))
    {
        Serial.println(F("Victron idle: no new telemetry."));
        lastVictronActivityMillis = millis();
    }
}

void SensorService::parseVictronLine(const String& line)
{
    int separatorIndex = line.indexOf(' ');
    if (separatorIndex < 0)
    {
        return;
    }

    String field = line.substring(0, separatorIndex);
    String valueText = line.substring(separatorIndex + 1);

    if (field == "V")
    {
        lastVoltage = parseVictronValue(valueText, true);
    }
    else if (field == "I")
    {
        lastCurrent = parseVictronValue(valueText, true);
    }
    else if (field == "VPV")
    {
        lastPanelVoltage = parseVictronValue(valueText, true);
    }
    else if (field == "PPV")
    {
        lastPanelPower = parseVictronValue(valueText, false);
    }
    else if (field == "CS")
    {
        lastChargerState = atoi(valueText.c_str());
    }
    else if (field == "ERR")
    {
        lastError = atoi(valueText.c_str());
    }
    else if (field == "H19")
    {
        lastGainedEnergyToday = parseVictronValue(valueText, false);
    }
}

float SensorService::parseVictronValue(const String& valueText, bool divideBy1000) const
{
    if (valueText.length() == 0)
    {
        return 0.0f;
    }

    float value = static_cast<float>(atof(valueText.c_str()));
    return divideBy1000 ? value / 1000.0f : value;
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

    readVictronTelemetry();

    data.temperature = readTemperature();
    data.voltage = lastVoltage;
    data.current = lastCurrent;
    data.panelVoltage = lastPanelVoltage;
    data.panelPower = lastPanelPower;
    data.chargerState = lastChargerState;
    data.error = lastError;
    data.gainedEnergyToday = lastGainedEnergyToday;

    data.lightAnalog = readLightAnalog();
    data.lightDigital = readLightDigital();

    return data;
}
