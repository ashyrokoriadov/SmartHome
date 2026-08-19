#include "App.h"
#include "Config.h"
#include "HomeAssistantPayload.h"
#include "HomeAssistantDiscovery.h"

App::App()
    : clockService(),
      sensorService(),
      lightingService(clockService, sensorService),
      victronService(clockService),
      mqttService(),
      lastSensorScanMs(0),
      lastVictronScanMs(0)
{
}

void App::setup()
{
    Serial.begin(115200);
    delay(1000);

    pinMode(LAMPS_CONTROL_PIN, OUTPUT);
    digitalWrite(LAMPS_CONTROL_PIN, LOW);

    bool wifiOk = connectToWifi();

    clockService.begin();
    sensorService.begin();
    victronService.begin();

    if (wifiOk) {
        if (!mqttService.begin(MQTT_BROKER, MQTT_PORT)) {
            Serial.println("Initial MQTT connect failed.");
        }
    } else {
        Serial.println("Skipping MQTT begin because WiFi is not connected.");
    }
    publishDiscovery();

    lastSensorScanMs = millis();
    lastVictronScanMs = millis();
}

void App::publishDiscovery()
{
    const char* baseTopic = MQTT_BASE_TOPIC;
    const char* deviceId = "smarthome_collector";

    // Temperature
    {
        const char* uniqueId = "balcony_temperature";
        String stateTopic = String(baseTopic) + "/sensor/temperature";
        String configTopic = String("homeassistant/sensor/") + deviceId + "/" + uniqueId + "/config";
        String payload = HomeAssistantDiscovery::buildSensorConfig(uniqueId, "Balcony Temperature", stateTopic.c_str(), "°C", "temperature");
        mqttService.publishJson(configTopic, payload);
    }

    // Humidity
    {
        const char* uniqueId = "balcony_humidity";
        String stateTopic = String(baseTopic) + "/sensor/humidity";
        String configTopic = String("homeassistant/sensor/") + deviceId + "/" + uniqueId + "/config";
        String payload = HomeAssistantDiscovery::buildSensorConfig(uniqueId, "Balcony Humidity", stateTopic.c_str(), "%", "humidity");
        mqttService.publishJson(configTopic, payload);
    }

    // Pressure
    {
        const char* uniqueId = "balcony_pressure";
        String stateTopic = String(baseTopic) + "/sensor/pressure";
        String configTopic = String("homeassistant/sensor/") + deviceId + "/" + uniqueId + "/config";
        String payload = HomeAssistantDiscovery::buildSensorConfig(uniqueId, "Balcony Pressure", stateTopic.c_str(), "hPa", "pressure");
        mqttService.publishJson(configTopic, payload);
    }

    // Light
    {
        const char* uniqueId = "balcony_light";
        String stateTopic = String(baseTopic) + "/sensor/light";
        String configTopic = String("homeassistant/sensor/") + deviceId + "/" + uniqueId + "/config";
        String payload = HomeAssistantDiscovery::buildSensorConfig(uniqueId, "Balcony Light", stateTopic.c_str(), "lx", "illuminance");
        mqttService.publishJson(configTopic, payload);
    }

    // AQI
    {
        const char* uniqueId = "balcony_aqi";
        String stateTopic = String(baseTopic) + "/sensor/aqi";
        String configTopic = String("homeassistant/sensor/") + deviceId + "/" + uniqueId + "/config";
        String payload = HomeAssistantDiscovery::buildSensorConfig(uniqueId, "Balcony AQI", stateTopic.c_str(), "AQI", "aqi");
        mqttService.publishJson(configTopic, payload);
    }

    // eCO2
    {
        const char* uniqueId = "balcony_eco2";
        String stateTopic = String(baseTopic) + "/sensor/eco2";
        String configTopic = String("homeassistant/sensor/") + deviceId + "/" + uniqueId + "/config";
        String payload = HomeAssistantDiscovery::buildSensorConfig(uniqueId, "Balcony eCO2", stateTopic.c_str(), "ppm", "carbon_dioxide");
        mqttService.publishJson(configTopic, payload);
    }

    // TVOC
    {
        const char* uniqueId = "balcony_tvoc";
        String stateTopic = String(baseTopic) + "/sensor/tvoc";
        String configTopic = String("homeassistant/sensor/") + deviceId + "/" + uniqueId + "/config";
        String payload = HomeAssistantDiscovery::buildSensorConfig(uniqueId, "Balcony TVOC", stateTopic.c_str(), "ppb", "volatile_organic_compounds");
        mqttService.publishJson(configTopic, payload);
    }

	// Victron PV Voltage
	{
		const char* uniqueId = "balcony_pv_voltage";
		String stateTopic = String(baseTopic) + "/sensor/pv_voltage";
		String configTopic = String("homeassistant/sensor/") + deviceId + "/" + uniqueId + "/config";
		String payload = HomeAssistantDiscovery::buildSensorConfig(uniqueId, "Balcony PV Voltage", stateTopic.c_str(), "V", "voltage");
		mqttService.publishJson(configTopic, payload);
	}

	// Victron Total Energy
	{
		const char* uniqueId = "balcony_total_energy";
		String stateTopic = String(baseTopic) + "/sensor/total_energy";
		String configTopic = String("homeassistant/sensor/") + deviceId + "/" + uniqueId + "/config";
		String payload = HomeAssistantDiscovery::buildSensorConfig(uniqueId, "Balcony Total Energy", stateTopic.c_str(), "kWh", "energy");
		mqttService.publishJson(configTopic, payload);
	}
}

void App::loop()
{
    if (WiFi.status() != WL_CONNECTED) {
        connectToWifi();
    }

    mqttService.loop();
    mqttService.reconnectIfNeeded();
    lightingService.update();

    const unsigned long nowMs = millis();
    if (nowMs - lastSensorScanMs >= SENSOR_INTERVAL_MS) {
        lastSensorScanMs = nowMs;
        publishSensorData();
    }

    if (nowMs - lastVictronScanMs >= VICRON_INTERVAL_MS) {
        lastVictronScanMs = nowMs;
        victronService.update();
        publishVictronData();
    }
}

bool App::connectToWifi()
{
    Serial.println("Connecting to WiFi");
    
    IPAddress ip(192, 168, 1, 217);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);
    IPAddress dns(8, 8, 8, 8);

    WiFi.config(ip, dns, gateway, subnet);

    const unsigned long WIFI_CONNECT_TIMEOUT_MS = 30000UL;
    unsigned long start = millis();

    int attempt = 0;
    while (WiFi.begin(WIFI_SSID, WIFI_PASSWORD) != WL_CONNECTED) {
        attempt++;
        Serial.print(".");
        Serial.print(" attempt=");
        Serial.print(attempt);
        Serial.print(" status=");
        Serial.println(WiFi.status());
        delay(WIFI_RETRY_MS);
        if (millis() - start >= WIFI_CONNECT_TIMEOUT_MS) {
            Serial.println("\nWiFi connect timeout");
            return false;
        }
    }

    Serial.println("\nWiFi connected");
    return true;
}

void App::publishSensorData()
{
    SensorData data = sensorService.readAll();

    const char* baseTopic = MQTT_BASE_TOPIC;
    char timestamp[32];
    clockService.formatUtc(timestamp, sizeof(timestamp));

    SensorReading temperatureReading { "balcony_temperature", "Balcony Temperature", "/smarthome/balcony/temperature", "°C", "temperature", data.temperature };
    SensorReading humidityReading { "balcony_humidity", "Balcony Humidity", "/smarthome/balcony/humidity", "%", "humidity", data.humidity };
    SensorReading pressureReading { "balcony_pressure", "Balcony Pressure", "/smarthome/balcony/pressure", "hPa", "pressure", data.pressure };
    SensorReading lightReading { "balcony_light", "Balcony Light", "/smarthome/balcony/light", "lx", "illuminance", static_cast<float>(data.lightAnalog) };
    SensorReading airQualityReading { "balcony_aqi", "Balcony AQI", "/smarthome/balcony/aqi", "AQI", "aqi", data.airQualityIndex };
    SensorReading eco2Reading { "balcony_eco2", "Balcony eCO2", "/smarthome/balcony/eco2", "ppm", "carbon_dioxide", data.eco2 };
    SensorReading tvocReading { "balcony_tvoc", "Balcony TVOC", "/smarthome/balcony/tvoc", "ppb", "volatile_organic_compounds", data.tvoc };

    mqttService.publishJson(String(baseTopic) + "/sensor/temperature", HomeAssistantPayloadBuilder::build(temperatureReading));
    mqttService.publishJson(String(baseTopic) + "/sensor/humidity", HomeAssistantPayloadBuilder::build(humidityReading));
    mqttService.publishJson(String(baseTopic) + "/sensor/pressure", HomeAssistantPayloadBuilder::build(pressureReading));
    mqttService.publishJson(String(baseTopic) + "/sensor/light", HomeAssistantPayloadBuilder::build(lightReading));
    mqttService.publishJson(String(baseTopic) + "/sensor/aqi", HomeAssistantPayloadBuilder::build(airQualityReading));
    mqttService.publishJson(String(baseTopic) + "/sensor/eco2", HomeAssistantPayloadBuilder::build(eco2Reading));
    mqttService.publishJson(String(baseTopic) + "/sensor/tvoc", HomeAssistantPayloadBuilder::build(tvocReading));

    (void)timestamp;
}

void App::publishVictronData()
{
    const float pvVoltage = victronService.pvVoltage();
    const float totalEnergy = victronService.totalEnergy();
    const char* baseTopic = MQTT_BASE_TOPIC;

    if (pvVoltage > 0.0f) {
        SensorReading pvReading { "balcony_pv_voltage", "Balcony PV Voltage", "/smarthome/balcony/pv_voltage", "V", "voltage", pvVoltage };
        mqttService.publishJson(String(baseTopic) + "/sensor/pv_voltage", HomeAssistantPayloadBuilder::build(pvReading));
    }

    if (totalEnergy > 0.0f) {
        SensorReading energyReading { "balcony_total_energy", "Balcony Total Energy", "/smarthome/balcony/total_energy", "kWh", "energy", totalEnergy };
        mqttService.publishJson(String(baseTopic) + "/sensor/total_energy", HomeAssistantPayloadBuilder::build(energyReading));
    }
}
