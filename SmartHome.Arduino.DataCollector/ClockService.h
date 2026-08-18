#pragma once

#include <Arduino.h>
#include <RTClib.h>

class ClockService {
public:
    bool begin();
    DateTime now();
    void formatUtc(char* output, size_t outputSize);

private:
    RTC_DS3231 rtc;
};
