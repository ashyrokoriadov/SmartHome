#pragma once

#include <Arduino.h>
#include <WiFiS3.h>
#include <PubSubClient.h>

class MqttService {
public:
    bool begin(const char* brokerHost, uint16_t port);
    bool publishJson(const String& topic, const String& payload);
    void reconnectIfNeeded();
    void loop();

private:
    WiFiClient wifiClient;
    PubSubClient client;
    bool connected;
};
