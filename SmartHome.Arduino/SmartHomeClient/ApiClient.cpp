#include <Arduino.h>
#include <WiFiS3.h>

#include "ApiClient.h"
#include "Config.h"

bool ApiClient::begin()
{
    return true;
}

bool ApiClient::get(
    const char* endpoint,
    char* response,
    size_t responseSize)
{
    WiFiClient client;

    if (!client.connect(API_HOST, API_PORT))
    {
        return false;
    }

    client.print("GET ");
    client.print(endpoint);
    client.println(" HTTP/1.1");

    client.print("Host: ");
    client.println(API_HOST);

    client.println("Connection: close");
    client.println();

    return readResponse(client, response, responseSize);
}

bool ApiClient::post(
    const char* endpoint,
    const char* payload)
{
    WiFiClient client;

    if (!client.connect(API_HOST, API_PORT))
    {
        return false;
    }

    client.print("POST ");
    client.print(endpoint);
    client.println(" HTTP/1.1");

    client.print("Host: ");
    client.println(API_HOST);

    client.println("Content-Type: application/json");

    client.print("Content-Length: ");
    client.println(strlen(payload));

    client.println("Connection: close");
    client.println();

    client.print(payload);

    return true;
}

bool ApiClient::healthCheck()
{
    char response[32];

    return get("/HealthCheck", response, sizeof(response));
}

bool ApiClient::readResponse(
    WiFiClient& client,
    char* response,
    size_t responseSize)
{
    unsigned long timeout = millis();

    while (!client.available())
    {
        if (millis() - timeout > 5000)
        {
            client.stop();
            return false;
        }
    }

    // ---- skip HTTP headers ----
    String line;
    while (client.connected())
    {
        line = client.readStringUntil('\n');
        if (line == "\r" || line.length() <= 1)
            break;
    }

    memset(response, 0, responseSize);

    size_t index = 0;

    // IMPORTANT FIX: read until connection closes, NOT available()
    while (client.connected())
    {
        if (!client.available())
            continue;

        char c = client.read();

        if (index < responseSize - 1)
        {
            // only keep printable chars
            if (c >= 32 && c <= 126)
            {
                response[index++] = c;
            }
        }
    }

    response[index] = '\0';

    trimGarbage(response);

    client.stop();
    return true;
}

void ApiClient::trimGarbage(char* str)
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

    // Remove quotes from beginning and end if present
    len = strlen(str);
    if (len > 0 && str[0] == '"')
    {
        // shift left to remove opening quote
        for (size_t i = 0; i < len - 1; i++)
        {
            str[i] = str[i + 1];
        }
        len--;
    }

    if (len > 0 && str[len - 1] == '"')
    {
        // remove closing quote
        str[len - 1] = '\0';
    }
}