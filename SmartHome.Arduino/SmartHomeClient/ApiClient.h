#pragma once

#include <Arduino.h>
#include <WiFiS3.h>

class ApiClient {
public:
    bool begin();
    bool healthCheck();

    bool get(
        const char* endpoint,
        char* response,
        size_t responseSize
    );

    bool post(
        const char* endpoint,
        const char* payload
    );

private:
    bool readResponse(
        WiFiClient& client,
        char* response,
        size_t responseSize
    );
    void trimGarbage(char* str);
};