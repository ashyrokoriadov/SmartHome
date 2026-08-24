#include "ClockService.h"

bool ClockService::begin()
{
    if (!rtc.begin()) {
        Serial.println("RTC not found.");
        return false;
    }

    Serial.println("RTC initialized.");
    return true;
}

bool ClockService::now(DateTime& result)
{
    result = rtc.now();

    if (result.year() < 2020 || result.year() > 2100) {
        Serial.println("RTC returned invalid time.");
        return false;
    }

    return true;
}

void ClockService::formatUtc(char* output, size_t outputSize)
{
    DateTime nowValue;

    if (!now(nowValue)) {
        output[0] = '\0';
        return;
    }

    snprintf(
        output,
        outputSize,
        "%04d-%02d-%02dT%02d:%02d:%02dZ",
        nowValue.year(),
        nowValue.month(),
        nowValue.day(),
        nowValue.hour(),
        nowValue.minute(),
        nowValue.second()
    );
}