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

    Serial.println("RTC found.");
    return true;
}

void ClockService::syncIfNeeded(ApiClient apiClient)
{
    Serial.println("Trying to sync clock.");

    DateTime currentTime = rtc.now();
    Serial.print("Current time set is ");
    Serial.println(currentTime.timestamp());

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

    trimGarbage(utcDateTime);

    Serial.print("CLEANED: ");
    Serial.println(utcDateTime);

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

void ClockService::trimGarbage(char* str)
{
    size_t len = strlen(str);

    if (len <= 3)
    {
        str[0] = '\0';
        return;
    }

    // shift left by 2 chars
    for (size_t i = 0; i < len - 2; i++)
    {
        str[i] = str[i + 2];
    }

    // remove last character
    str[len - 3] = '\0';
}