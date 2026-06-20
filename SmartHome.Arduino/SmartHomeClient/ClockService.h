#pragma once

#include <Arduino.h>
#include <RTClib.h>

class ApiClient;

class ClockService {
public:
    bool begin();

    void syncIfNeeded(ApiClient& apiClient);

    DateTime now();

    void formatUtc(char* output, size_t outputSize);

private:
    RTC_DS3231 rtc;
};