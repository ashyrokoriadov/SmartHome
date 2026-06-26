#include <Arduino.h>
#include <RTClib.h>

#include "ClockService.h"
#include "ApiClient.h"

bool ClockService::begin()
{
    if (!rtc.begin())
    {
        Serial.println("RTC not found.");
        return false;
    }
    return true;
}

void ClockService::syncIfNeeded(ApiClient apiClient)
{
    DateTime currentTime = rtc.now();

    if (currentTime.year() >= 2002)
    {
        Serial.println("No need to sync the clock.");
        return;
    }

    char utcDateTime[128];
    memset(utcDateTime, 0, sizeof(utcDateTime));

    if (!apiClient.get(
            "/MetaData/DateTimeUtc",
            utcDateTime,
            sizeof(utcDateTime)))
    {
        Serial.println("Failed to get UTC time");
        return;
    }

    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;

    int parsed = sscanf(
        utcDateTime,
        "%d-%d-%d %d:%d:%d",
        &year,
        &month,
        &day,
        &hour,
        &minute,
        &second
    );

    if (parsed != 6)
    {
        Serial.print("Invalid datetime: ");
        Serial.println(utcDateTime);
        return;
    }

    rtc.adjust(DateTime(
        year,
        month,
        day,
        hour,
        minute,
        second
    ));

    Serial.println("RTC synchronized");
}

DateTime ClockService::now()
{
    return rtc.now();
}

void ClockService::formatUtc(
    char* output,
    size_t outputSize)
{
    DateTime current = rtc.now();

    snprintf(
        output,
        outputSize,
        "%04d-%02d-%02dT%02d:%02d:%02dZ",
        current.year(),
        current.month(),
        current.day(),
        current.hour(),
        current.minute(),
        current.second()
    );
}

