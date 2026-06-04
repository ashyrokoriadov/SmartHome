#include <WiFiS3.h>
#include <ArduinoJson.h>
#include <ArduinoGraphics.h>
#include <Arduino_LED_Matrix.h>
#include <TextAnimation.h>
#include <gallery.h>
#include <Wire.h>
#include <RTClib.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_INA219.h>

// =====================================================
// WIFI CONFIG
// =====================================================

char ssid[] = "SECRET_WI_FI";
char pass[] = "SECRET_PASSWORD";

// =====================================================
// API CONFIG
// =====================================================

const char serverHost[] = "192.168.1.94";
const int serverPort = 5000;

// =====================================================
// TIMER
// =====================================================

unsigned long lastSend = 0;
const unsigned long interval = 15 * 1000;
bool rtcInitialized = false;

RTC_DS3231 rtc;

// =====================================================
// TEMPERATURE
// =====================================================

#define ONE_WIRE_BUS 3

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// =====================================================
// GLOBAL METADATA
// =====================================================

char correlationId[128] = "";
char utcDateTime[128] = "";
char location[128] = "Balcony";

// =====================================================
// MATRIX
// =====================================================

ArduinoLEDMatrix matrix;

// =====================================================
// VOLTAGE / CURRENT
// =====================================================

Adafruit_INA219 ina219;

// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  matrix.begin();

  unsigned long start = millis();

  while (!Serial && millis() - start < 3000);

  Serial.println("=================================");
  Serial.println("Arduino HTTP JSON Sender");
  Serial.println("=================================");

  randomSeed(analogRead(A5));

  pinMode(D2, INPUT);

  connectWiFi();

  if (rtc.begin()) {
    Serial.println("Clock DS3231 found.");
    rtcInitialized = true;
   }
   else {
    Serial.println("WARNING: Clock DS3231 not found");
   }

  sensors.begin();
  Serial.println("Temperature sensor DS18B20 found.");

  if (ina219.begin()) {
    Serial.println("Voltage sensor INA219 found.");
  } else {    
    Serial.println("WARNING! Voltage sensor INA219 was not found.");
  }
}

// =====================================================
// MAIN LOOP
// =====================================================

void loop() {

  if (WiFi.status() != WL_CONNECTED) {

    Serial.println("WiFi disconnected!");

    matrixPrint("WIFI RECONNECT");

    connectWiFi();
  }

  if (millis() - lastSend >= interval) {

    lastSend = millis();

    // ==========================================
    // FETCH METADATA
    // ==========================================

    bool correlationOk = httpGet(
      "/MetaData/CorrelationId",
      correlationId,
      sizeof(correlationId)
    );

    bool dateOk = false;

    if (rtcInitialized){
      readTime();
      dateOk = true;
    } else {
      bool dateOk = httpGet(
          "/MetaData/DateTimeUtc",
          utcDateTime,
          sizeof(utcDateTime)
      );
    }  

    if (!correlationOk || !dateOk) {

      Serial.println("Metadata fetch failed");

      matrixPrint("METADATA ERROR");

      return;
    }

    // ==========================================
    // SEND SENSOR DATA
    // ==========================================

    readVoltage();
    readLuminosity();
    readTemperature();

    matrixPrint("OK. OK. OK.");
  }
}

// =====================================================
// WIFI CONNECT
// =====================================================

void connectWiFi() {

  Serial.print("Connecting to WiFi");

  int status = WL_IDLE_STATUS;

  while (status != WL_CONNECTED) {

    status = WiFi.begin(ssid, pass);

    Serial.print(".");

    delay(5000);
  }

  Serial.println("\nWiFi connected");

  IPAddress ip;

  do {

    delay(1000);

    ip = WiFi.localIP();

    Serial.print("Waiting for IP... ");
    Serial.println(ip);

    char ipBuffer[16];
    String ipStr = ip.toString();
    ipStr.toCharArray(ipBuffer, sizeof(ipBuffer));
    matrixPrint(ipBuffer);

  } while (ip[0] == 0);

  Serial.print("Device IP: ");
  Serial.println(ip);

  Serial.println("=================================");
}

// =====================================================
// EXTRACT QUOTED VALUE
// =====================================================

bool extractQuotedValue(
  const char* body,
  char* output,
  size_t outputSize) {

  const char* firstQuote = strchr(body, '\"');

  if (firstQuote == nullptr) {

    return false;
  }

  const char* secondQuote = strchr(firstQuote + 1, '\"');

  if (secondQuote == nullptr) {

    return false;
  }

  size_t len = secondQuote - firstQuote - 1;

  if (len >= outputSize) {

    len = outputSize - 1;
  }

  strncpy(output, firstQuote + 1, len);

  output[len] = '\0';

  return true;
}

// =====================================================
// GENERIC HTTP GET
// =====================================================

bool httpGet(
  const char* endpoint,
  char* output,
  size_t outputSize) {

  WiFiClient localClient;

  Serial.print("HTTP GET: ");
  Serial.println(endpoint);

  if (!localClient.connect(serverHost, serverPort)) {

    Serial.println("GET connection failed");

    return false;
  }

  // =====================================================
  // REQUEST
  // =====================================================

  localClient.print("GET ");
  localClient.print(endpoint);
  localClient.print(" HTTP/1.1\r\n");

  localClient.print("Host: ");
  localClient.print(serverHost);
  localClient.print(":");
  localClient.print(serverPort);
  localClient.print("\r\n");

  localClient.print("Connection: close\r\n\r\n");


  // =====================================================
  // RESPONSE BUFFER
  // =====================================================

  char response[1024];
  size_t index = 0;

  unsigned long timeout = millis();

  while (localClient.connected()) {

    while (localClient.available()) {

      char c = localClient.read();

      if (index < sizeof(response) - 1) {

        response[index++] = c;
      }

      timeout = millis();
    }

    if (millis() - timeout > 5000) {

      Serial.println("GET timeout");

      localClient.stop();

      return false;
    }
  }

  response[index] = '\0';

  localClient.stop();

  // =====================================================
  // FIND BODY
  // =====================================================

  char* body = strstr(response, "\r\n\r\n");

  if (body == nullptr) {

    Serial.println("Invalid HTTP response");

    return false;
  }

  body += 4;

  // =====================================================
  // PARSE VALUE
  // =====================================================

  if (!extractQuotedValue(body, output, outputSize)) {

    Serial.println("Failed to parse response");

    return false;
  }

  Serial.print("Parsed value: ");
  Serial.println(output);

  return true;
}

void readTime(){
  DateTime now = rtc.now();

  Serial.print(now.day());
  Serial.print(".");
  Serial.print(now.month());
  Serial.print(".");
  Serial.print(now.year());

  Serial.print(" ");

  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.println(now.second());

  snprintf(utcDateTime, sizeof(utcDateTime),
          "%04d-%02d-%02dT%02d:%02d:%02dZ",
          now.year(),
          now.month(),
          now.day(),
          now.hour(),
          now.minute(),
          now.second());
}

// =====================================================
// BATTERY SENSOR
// =====================================================

void readVoltage() {

  float voltage = ina219.getBusVoltage_V();
  float current = ina219.getCurrent_mA();

  Serial.print("Napiecie: ");
  Serial.print(voltage);
  Serial.println(" V");

  Serial.print("Prad: ");
  Serial.print(current);
  Serial.println(" mA");

  char jsonBuffer[512];

  StaticJsonDocument<512> doc;

  doc["correlationId"] = correlationId;
  doc["timestamp"] = utcDateTime;
  doc["location"] = location;

  doc["voltage"] = voltage;
  doc["current"] = current;

  serializeJson(doc, jsonBuffer);

  sendData("/Battery/Add", jsonBuffer);
}

// =====================================================
// LIGHT SENSOR
// =====================================================

void readLuminosity() {

  int analogueValue = analogRead(A0);
  int digitalValue = digitalRead(D2);

  Serial.print("AnalogueValue: ");
  Serial.println(analogueValue);

  Serial.print("DigitalValue: ");
  Serial.println(digitalValue);

  char jsonBuffer[512];

  StaticJsonDocument<512> doc;

  doc["correlationId"] = correlationId;
  doc["timestamp"] = utcDateTime;
  doc["location"] = location;

  doc["analogueValue"] = analogueValue;
  doc["digitalValue"] = digitalValue;

  serializeJson(doc, jsonBuffer);

  sendData("/LightSensor/Add", jsonBuffer);
}

// =====================================================
// TEMPERATURE SENSOR
// =====================================================

void readTemperature() {
  sensors.requestTemperatures();
  float temperature = sensors.getTempCByIndex(0);

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  char jsonBuffer[512];

  StaticJsonDocument<512> doc;

  doc["correlationId"] = correlationId;
  doc["timestamp"] = utcDateTime;
  doc["location"] = location;

  doc["temperature"] = temperature;

  serializeJson(doc, jsonBuffer);

  sendData("/Temperature/Add", jsonBuffer);
}

// =====================================================
// GENERIC HTTP POST METHOD
// =====================================================

bool sendData(
  const char* endpoint,
  const char* jsonPayload) {

  WiFiClient localClient;

  Serial.println("Connecting to API server...");

  bool connected = false;

  for (int i = 0; i < 3; i++) {

    if (localClient.connect(serverHost, serverPort)) {

      connected = true;
      break;
    }

    Serial.println("Retry connection...");

    delay(1000);
  }

  if (!connected) {

    Serial.println("CONNECTION FAILED");

    Serial.println("=================================");

    matrixPrint("CONNECTION FAILED");

    return false;
  }

  Serial.println("Connected to server");

  // =====================================================
  // HTTP REQUEST
  // =====================================================

  localClient.print("POST ");
  localClient.print(endpoint);
  localClient.print(" HTTP/1.1\r\n");

  localClient.print("Host: ");
  localClient.print(serverHost);
  localClient.print(":");
  localClient.print(serverPort);
  localClient.print("\r\n");

  localClient.print("Content-Type: application/json\r\n");
  localClient.print("Connection: close\r\n");

  localClient.print("Content-Length: ");
  localClient.print(strlen(jsonPayload));
  localClient.print("\r\n");

  localClient.print("\r\n");

  localClient.print(jsonPayload);

  Serial.println("JSON SENT:");
  Serial.println(jsonPayload);

  // =====================================================
  // RESPONSE
  // =====================================================

  unsigned long timeout = millis();

  while (localClient.connected()) {

    while (localClient.available()) {

      char c = localClient.read();

      Serial.print(c);

      timeout = millis();
    }

    if (millis() - timeout > 5000) {

      Serial.println("SERVER TIMEOUT");

      localClient.stop();

      Serial.println("=================================");

      matrixPrint("SERVER TIMEOUT");

      return false;
    }
  }

  localClient.stop();

  Serial.println();
  Serial.println("Disconnected from server");
  Serial.println("=================================");

  return true;
}

// =====================================================
// PRINT IN MATRIX
// =====================================================

void matrixPrint(const char* message) {

  matrix.clear();

  matrix.beginDraw();

  matrix.stroke(0xFFFFFF);

  matrix.textScrollSpeed(160);

  matrix.textFont(Font_5x7);

  matrix.beginText(0, 1, 0xFFFFFF);

  matrix.print(message);

  matrix.endText(SCROLL_LEFT);

  matrix.endDraw();
}