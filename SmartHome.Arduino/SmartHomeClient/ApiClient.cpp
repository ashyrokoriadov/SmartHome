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

    client.setTimeout(3000);

    client.print("GET ");
    client.print(endpoint);
    client.println(" HTTP/1.1");

    client.print("Host: ");
    client.println(API_HOST);

    client.println("Connection: close");
    client.println();

    int statusCode = 0;
    bool result = readResponse(client, response, responseSize, &statusCode);
    Serial.print("Status code GET: ");
    Serial.println(statusCode);
    return result && statusCode >= 200 && statusCode < 300;
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

    client.setTimeout(3000);

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

    size_t written = client.print(payload);
    client.println();
    client.flush();

    bool sent = written > 0;

    if (!sent)
    {
        client.stop();
        return false;
    }

    unsigned long timeout = millis();
    while (!client.available() && millis() - timeout < 5000)
    {
        delay(10);
    }

    if (!client.available())
    {
        client.stop();
        return false;
    }

    String statusLine = client.readStringUntil('\n');
    if (statusLine.length() == 0)
    {
        client.stop();
        return false;
    }

    int statusCode = 0;
    int spaceIndex = statusLine.indexOf(' ');
    if (spaceIndex >= 0)
    {
        String codeText = statusLine.substring(spaceIndex + 1, spaceIndex + 4);
        statusCode = codeText.toInt();
    }

    while (client.connected())
    {
        if (client.available())
        {
            client.read();
        }
        else
        {
            delay(10);
        }
    }

    client.stop();
    Serial.print("Status code POST:");
    Serial.println(statusCode);
    return statusCode >= 200 && statusCode < 300;
}

bool ApiClient::healthCheck()
{
    char response[32];

    return get("/HealthCheck", response, sizeof(response));
}

bool ApiClient::readResponse(
    WiFiClient& client,
    char* response,
    size_t responseSize,
    int* statusCode)
{
    unsigned long timeout = millis();

    while (!client.available() && millis() - timeout < 5000)
    {
        delay(10);
    }

    if (!client.available())
    {
        if (statusCode)
        {
            *statusCode = 0;
        }
        client.stop();
        return false;
    }

    String statusLine = client.readStringUntil('\n');
    if (statusLine.length() == 0)
    {
        if (statusCode)
        {
            *statusCode = 0;
        }
        client.stop();
        return false;
    }

    if (statusCode)
    {
        int spaceIndex = statusLine.indexOf(' ');
        if (spaceIndex >= 0)
        {
            String codeText = statusLine.substring(spaceIndex + 1, spaceIndex + 4);
            *statusCode = codeText.toInt();
        }
        else
        {
            *statusCode = 0;
        }
    }

    // ---- skip HTTP headers ----
    String line;
    while (client.connected() && millis() - timeout < 5000)
    {
        line = client.readStringUntil('\n');
        if (line == "\r" || line.length() <= 1)
            break;
    }

    memset(response, 0, responseSize);

    size_t index = 0;
    bool sawData = false;

    while (millis() - timeout < 5000)
    {
        while (client.available())
        {
            char c = static_cast<char>(client.read());
            sawData = true;

            if (index < responseSize - 1)
            {
                if (c >= 32 && c <= 126)
                {
                    response[index++] = c;
                }
            }
        }

        if (!client.connected() && !client.available())
        {
            break;
        }

        delay(10);
    }

    response[index] = '\0';

    trimGarbage(response);

    client.stop();
    return sawData;
}

void ApiClient::trimGarbage(char* str)
{
    if (str == nullptr)
    {
        return;
    }

    size_t len = strlen(str);
    if (len < 2)
    {
        return;
    }

    size_t start = 0;
    size_t end = len;

    if (len >= 2)
    {
        start = 2;
    }

    if (end >= 1)
    {
        end -= 1;
    }

    if (start >= end)
    {
        str[0] = '\0';
        return;
    }

    size_t contentLen = end - start;
    if (contentLen > 0)
    {
        memmove(str, str + start, contentLen);
    }

    str[contentLen] = '\0';

    len = strlen(str);
    if (len >= 2 && str[0] == '"' && str[len - 1] == '"')
    {
        memmove(str, str + 1, len - 2);
        str[len - 2] = '\0';
    }
}