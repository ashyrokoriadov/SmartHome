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
        Serial.println("Connection failed");
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
        Serial.println("Connection failed");
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

    return get("/health", response, sizeof(response));
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

    bool bodyStarted = false;
    size_t index = 0;

    while (client.connected() || client.available())
    {
        String line = client.readStringUntil('\n');

        if (!bodyStarted)
        {
            if (line == "\r")
            {
                bodyStarted = true;
            }

            continue;
        }

        line.trim();

        for (size_t i = 0; i < line.length(); i++)
        {
            if (index < responseSize - 1)
            {
                response[index++] = line[i];
            }
        }
    }

    response[index] = '\0';

    client.stop();

    return true;
}