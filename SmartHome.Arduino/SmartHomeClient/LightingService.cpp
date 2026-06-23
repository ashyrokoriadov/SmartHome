#pragma once

#include <Arduino.h>
#include "LightingService.h"
#include "Config.h"

void LightingService::toggleLightIfPossible()
{
    DateTime currentTime = clockService.now();
    bool lightSensorDigital = sensorService.readLightDigital();

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