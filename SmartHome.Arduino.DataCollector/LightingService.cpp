#include "LightingService.h"
#include "Config.h"

#define RTC_FAILURE_TIMEOUT_MS 5000UL

LightingService::LightingService(ClockService& clock, SensorService& sensor)
    : clockService(clock),
    sensorService(sensor),
    timeCondition(false),
    lightCondition(false),
    changed(false),
    turnedOn(false),
    rtcFailureActive(false),
    rtcFailureStart(0)
{}

void LightingService::update()
{
    DateTime nowValue;

    if (!clockService.now(nowValue)) {

        if (!rtcFailureActive) {
            rtcFailureActive = true;
            rtcFailureStart = millis();

            Serial.println("RTC unavailable - keeping current lamp state.");
        }

        if (millis() - rtcFailureStart < RTC_FAILURE_TIMEOUT_MS) {
            return;
        }

        if (turnedOn) {
            turnedOn = false;

            digitalWrite(LAMPS_CONTROL_PIN, LOW);

            Serial.println("RTC failure timeout - lamps OFF.");
        }

        return;
    }

    if (rtcFailureActive) {
        rtcFailureActive = false;

        Serial.println("RTC recovered.");
    }

    int hour = nowValue.hour();

    bool newTimeCondition = (hour >= 16 && hour <= 21);
    bool newLightCondition = (sensorService.readLightDigital() == 1);

    changed = (timeCondition != newTimeCondition) ||
        (lightCondition != newLightCondition);

    timeCondition = newTimeCondition;
    lightCondition = newLightCondition;

    turnedOn = timeCondition && lightCondition;

    digitalWrite(
        LAMPS_CONTROL_PIN,
        turnedOn ? HIGH : LOW
    );

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