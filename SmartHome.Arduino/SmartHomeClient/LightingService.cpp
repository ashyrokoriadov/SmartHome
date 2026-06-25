#pragma once

#include <Arduino.h>
#include "LightingService.h"
#include "Config.h"

LightingService::LightingService(ClockService& clock)
    : clockService(clock)
{
    // optional initialization code
}

void LightingService::toggleLightIfPossible()
{

    DateTime currentTime = clockService.now();
    Serial.print("LightingService - currentTime: ");
    Serial.println(currentTime.timestamp());

    bool lightSensorDigital = sensorService.readLightDigital();
    Serial.print("LightingService - lightSensorDigital: ");
    Serial.println(lightSensorDigital);

    int hour = currentTime.hour();

    bool timeCondition = (hour >= 17 && hour <= 21);
    bool lightCondition = (lightSensorDigital == 1);

    if (timeCondition && lightCondition) {
        digitalWrite(LAMPS_CONTROL_PIN, HIGH);

        Serial.println("Lamps are turned on.");
        Serial.print("timeCondition = ");
        Serial.println(timeCondition);
        Serial.print("lightCondition = ");
        Serial.println(lightCondition);
    }
    else {
        digitalWrite(LAMPS_CONTROL_PIN, LOW);

        Serial.println("Lamps are turned off.");
        Serial.print("timeCondition = ");
        Serial.println(timeCondition);
        Serial.print("lightCondition = ");
        Serial.println(lightCondition);
    }
}