#pragma once

#include <Arduino.h>
#include "LightingService.h"
#include "Config.h"

LightingService::LightingService(ClockService& clock)
    : clockService(clock)
{ }

bool timeCondition = false;
bool lightCondition = false;
bool changed = false;
bool turnedOn = false;

void LightingService::toggleLightIfPossible()
{
    DateTime currentTime = clockService.now();

    bool lightSensorDigital = sensorService.readLightDigital();

    int hour = currentTime.hour();

    bool timeConditionActual = (hour >= 16 && hour < 20);
    bool lightConditionActual = (lightSensorDigital == 1);

    if((timeCondition != timeConditionActual) || (lightCondition != lightConditionActual))
    {
        changed = true;
    }
    else
    {
        changed = false;
    }

    timeCondition = timeConditionActual;
    lightCondition = lightConditionActual;
    turnedOn = timeCondition && lightCondition;

    if (turnedOn) {
        digitalWrite(LAMPS_CONTROL_PIN, HIGH);
    }
    else {
        digitalWrite(LAMPS_CONTROL_PIN, LOW);
    }

    if (changed)
    {
        Serial.print("Lamps are turned = ");
        Serial.print(turnedOn);
        Serial.print("; timeCondition = ");
        Serial.print(timeCondition);
        Serial.print("; lightCondition = ");
        Serial.print(lightCondition);
        Serial.println(".");
    }        
}