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

DateTime ClockService::now()
{
    return rtc.now();
}

void ClockService::formatUtc(char* output, size_t outputSize)
{
    DateTime nowValue = rtc.now();
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
