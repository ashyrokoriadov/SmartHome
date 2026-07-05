#include <Arduino.h>
#include <SoftwareSerial.h>
#include <string.h>

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

    //Serial1.begin(19200);

    return true;
}

void SensorService::readVictronTelemetry()
{
    static bool hasLoggedVictronData = false;
    static unsigned long lastVictronLogMillis = 0;
    static unsigned long lastVictronActivityMillis = 0;

    const unsigned long victronLogInterval = 2000UL;
    const unsigned long victronIdleLogInterval = 10000UL;

    char line[48];
    size_t lineLength = 0;
    bool receivedData = false;

    while (victronSerial.available())
    {
        receivedData = true;
        lastVictronActivityMillis = millis();

        char c = static_cast<char>(victronSerial.read());

        if (c == '\n' || c == '\r')
        {
            if (lineLength > 0)
            {
                line[lineLength] = '\0';
                parseVictronLine(line);
                lineLength = 0;
            }
        }
        else if (lineLength < sizeof(line) - 1)
        {
            line[lineLength++] = c;
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

void SensorService::parseVictronLine(const char* line)
{
    if (line == nullptr || line[0] == '\0')
    {
        return;
    }

    char field[8];
    char valueText[24];

    const char* separator = strchr(line, '\t');
    if (separator == nullptr)
    {
        separator = strchr(line, ' ');
    }

    if (separator == nullptr)
    {
        return;
    }

    const size_t fieldLength = static_cast<size_t>(separator - line);
    if (fieldLength == 0 || fieldLength >= sizeof(field))
    {
        return;
    }

    memcpy(field, line, fieldLength);
    field[fieldLength] = '\0';

    const char* value = separator + 1;
    strncpy(valueText, value, sizeof(valueText) - 1);
    valueText[sizeof(valueText) - 1] = '\0';

    if (strcmp(field, "V") == 0)
    {
        lastVoltage = parseVictronValue(valueText, true);
    }
    else if (strcmp(field, "I") == 0)
    {
        lastCurrent = parseVictronValue(valueText, true);
    }
    else if (strcmp(field, "VPV") == 0)
    {
        lastPanelVoltage = parseVictronValue(valueText, true);
    }
    else if (strcmp(field, "PPV") == 0)
    {
        lastPanelPower = parseVictronValue(valueText, false);
    }
    else if (strcmp(field, "CS") == 0)
    {
        lastChargerState = atoi(valueText);
    }
    else if (strcmp(field, "ERR") == 0)
    {
        lastError = atoi(valueText);
    }
    else if (strcmp(field, "H19") == 0)
    {
        lastGainedEnergyToday = parseVictronValue(valueText, false);
    }
}

float SensorService::parseVictronValue(const char* valueText, bool divideBy1000) const
{
    if (valueText == nullptr || valueText[0] == '\0')
    {
        return 0.0f;
    }

    const float value = static_cast<float>(atof(valueText));
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
