#include "TimeScheduleLightingService.h"
#include "Config.h"

#define RTC_FAILURE_TIMEOUT_MS 5000UL

TimeScheduleLightingService::TimeScheduleLightingService(ClockService& clock)
    : clockService(clock),
      timeCondition(false),
      changed(false),
      turnedOn(false),
      rtcFailureActive(false),
      rtcFailureStart(0)
{}

void TimeScheduleLightingService::update()
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

    const int hour = nowValue.hour();
    const bool newTimeCondition = (hour >= 16 && hour <= 21);

    changed = (timeCondition != newTimeCondition);
    timeCondition = newTimeCondition;
    turnedOn = newTimeCondition;

    digitalWrite(LAMPS_CONTROL_PIN, turnedOn ? HIGH : LOW);

    if (changed) {
        Serial.print("Lamps = ");
        Serial.print(turnedOn ? "ON" : "OFF");
        Serial.print("; time=");
        Serial.println(newTimeCondition ? "true" : "false");
    }
}
