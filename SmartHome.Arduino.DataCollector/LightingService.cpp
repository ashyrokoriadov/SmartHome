#include "LightingService.h"
#include "Config.h"

LightingService::LightingService(ClockService& clock, SensorService& sensor)
    : clockService(clock), sensorService(sensor), timeCondition(false), lightCondition(false), changed(false), turnedOn(false)
{
}

void LightingService::update()
{
    DateTime nowValue = clockService.now();
    int hour = nowValue.hour();

    bool newTimeCondition = (hour >= 16 && hour <= 19);
    bool newLightCondition = (sensorService.readLightDigital() == 1);

    changed = (timeCondition != newTimeCondition) || (lightCondition != newLightCondition);
    timeCondition = newTimeCondition;
    lightCondition = newLightCondition;
    turnedOn = timeCondition && lightCondition;

    digitalWrite(LAMPS_CONTROL_PIN, turnedOn ? HIGH : LOW);

    if (changed) {
        Serial.print("Lamps = ");
        Serial.print(turnedOn ? "ON" : "OFF");
        Serial.print("; time=");
        Serial.print(newTimeCondition ? "true" : "false");
        Serial.print("; daylight=");
        Serial.print(newLightCondition ? "true" : "false");
        Serial.println();
    }
}
