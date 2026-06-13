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
// CONTROL PINS
// =====================================================

int lightSensorControlPin = 2;
int lampsControlPin = 7;

// =====================================================
// SENSOR DATA
// =====================================================

float temperature = -127;
int lightSensorAnalogue = 0;
int lightSensorDigital = 0;
float voltage = 0;
float current = 0;
DateTime currentTime;

// =====================================================
// WIFI CONFIG
// =====================================================

char ssid[] = "SECRET";
char pass[] = "SECRET";

// =====================================================
// API CONFIG
// =====================================================

const char serverHost[] = "192.168.1.94";
const int serverPort = 5000;

// =====================================================
// TIMER
// =====================================================

unsigned long lastSend = 0;
unsigned long interval = 15 * 1000;
bool rtcInitialized = false;

RTC_DS3231 rtc;

// =====================================================
// TEMPERATURE
// =====================================================

#define ONE_WIRE_BUS 4

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

  pinMode(lightSensorControlPin, INPUT);
  pinMode(lampsControlPin, OUTPUT);
  digitalWrite(lightSensorControlPin, LOW);

  matrix.begin();

  unsigned long start = millis();

  while (!Serial && millis() - start < 3000);

  randomSeed(analogRead(A5));

  connectWiFi();

  if (rtc.begin()) {
    rtcInitialized = true;
   }
   else {
    Serial.println("WARNING: Clock DS3231 not found");
   }

  initializeSensors();

  if (!ina219.begin()) {    
    Serial.println("WARNING! Voltage sensor INA219 was not found.");
  }
}

// =====================================================
// MAIN LOOP
// =====================================================

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected!");
    connectWiFi();
  }

  if (millis() - lastSend >= interval) {

    lastSend = millis();

    toggleLightIfPossible();

    // ==========================================
    // FETCH METADATA
    // ==========================================

    bool isHealthy = healthCheck();
    if (!isHealthy)
    {
      matrixPrintStatic("E0");
      interval = 15 * 60 * 1000;
      return;
    }
    else
    {
      interval = 15 * 1000;
    }

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

      matrixPrintStatic("E1");

      return;
    }

    // ==========================================
    // SEND SENSOR DATA
    // ==========================================

    readVoltage();
    readLuminosity(true);
    readTemperature();

    matrixPrintStatic("OK");
  }
}

// =====================================================
//INITIALIZE SENSORS
// =====================================================

void initializeSensors(){
  sensors.begin();

  Serial.print("Devices found: ");
  Serial.println(sensors.getDeviceCount());

  DeviceAddress addr;

  if (sensors.getAddress(addr, 0)) {
      Serial.println("Sensor detected");

      for (uint8_t i = 0; i < 8; i++) {
          if (addr[i] < 16) Serial.print("0");
          Serial.print(addr[i], HEX);          
      }

      Serial.println();
  } else {
      Serial.println("No sensor found");
  }
}

// =====================================================
// WIFI CONNECT
// =====================================================

void connectWiFi() {

  Serial.println("Connecting to WiFi");

  int status = WL_IDLE_STATUS;

  while (status != WL_CONNECTED) {
    Serial.println("Connecting...");
    status = WiFi.begin(ssid, pass);
    delay(5000);
  }

  IPAddress ip;

  do {
    delay(1000);
    ip = WiFi.localIP();
    Serial.print("Waiting for IP... ");
  } while (ip[0] == 0);

  char ipBuffer[16];
  String ipStr = ip.toString();
  ipStr.toCharArray(ipBuffer, sizeof(ipBuffer)); 
  matrixPrint(ipBuffer);

  Serial.print("Device IP: ");
  Serial.println(ip);
}

// =====================================================
// EXTRACT QUOTED VALUE
// =====================================================

void toggleLightIfPossible(){
  readTime();
  readLuminosity(false);

  int hour = currentTime.hour();

  bool timeCondition = (hour >= 19 && hour <= 23);
  bool lightCondition = (lightSensorDigital == 1);

  if (timeCondition || lightCondition) {
    digitalWrite(lampsControlPin, HIGH);
    Serial.println("Lamps are turned on. ");
    Serial.print("timeCondition = ");
    Serial.println(timeCondition);
    Serial.print("lightCondition = ");
    Serial.println(lightCondition);
  } else {
    digitalWrite(lampsControlPin, LOW);
    Serial.println("Lamps are turned off.");
    Serial.print("timeCondition = ");
    Serial.println(timeCondition);
    Serial.print("lightCondition = ");
    Serial.println(lightCondition);
  }

}

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
// HEALTH CHECK
// =====================================================

bool healthCheck()
{
    WiFiClient localClient;

    if (!localClient.connect(serverHost, serverPort))
    {
        Serial.println("Connection failed");
        return false;
    }

    localClient.print("GET ");
    localClient.print("/HealthCheck");
    localClient.print(" HTTP/1.1\r\n");

    localClient.print("Host: ");
    localClient.print(serverHost);
    localClient.print(":");
    localClient.print(serverPort);
    localClient.print("\r\n");

    localClient.print("Connection: close\r\n\r\n");

    String statusLine = localClient.readStringUntil('\n');

    Serial.print("Status: ");
    Serial.println(statusLine);

    localClient.stop();

    return statusLine.indexOf("200") >= 0;
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
  return true;
}

void readTime(){
  currentTime = rtc.now();

  Serial.print(currentTime.day());
  Serial.print(".");
  Serial.print(currentTime.month());
  Serial.print(".");
  Serial.print(currentTime.year());

  Serial.print(" ");

  Serial.print(currentTime.hour());
  Serial.print(":");
  Serial.print(currentTime.minute());
  Serial.print(":");
  Serial.println(currentTime.second());

  snprintf(utcDateTime, sizeof(utcDateTime),
          "%04d-%02d-%02dT%02d:%02d:%02dZ",
          currentTime.year(),
          currentTime.month(),
          currentTime.day(),
          currentTime.hour(),
          currentTime.minute(),
          currentTime.second());
}

// =====================================================
// BATTERY SENSOR
// =====================================================

void readVoltage() {

  voltage = ina219.getBusVoltage_V();
  current = ina219.getCurrent_mA();

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

void readLuminosity(const bool sendDataToApi) {

  lightSensorAnalogue = analogRead(A0);
  lightSensorDigital = digitalRead(lightSensorControlPin);

  Serial.print("AnalogueValue: ");
  Serial.println(lightSensorAnalogue);

  Serial.print("DigitalValue: ");
  Serial.println(lightSensorDigital);

  if (sendDataToApi) {
  char jsonBuffer[512];

  StaticJsonDocument<512> doc;

  doc["correlationId"] = correlationId;
  doc["timestamp"] = utcDateTime;
  doc["location"] = location;

  doc["analogueValue"] = lightSensorAnalogue;
  doc["digitalValue"] = lightSensorDigital;

  serializeJson(doc, jsonBuffer);

  sendData("/LightSensor/Add", jsonBuffer);
  }
}

// =====================================================
// TEMPERATURE SENSOR
// =====================================================

void readTemperature() {
  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);

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
    matrixPrint("CONNECTION FAILED");

    return false;
  }

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
  return true;
}

// =====================================================
// PRINT IN MATRIX WITH SCROLL
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

// =====================================================
// PRINT IN MATRIX WITH SCROLL
// =====================================================

void matrixPrintStatic(const char* message)
{
    matrix.clear();
    matrix.beginDraw();
    matrix.stroke(0xFFFFFF);
    matrix.textFont(Font_5x7);
    int textWidth = strlen(message) * 6; // 5 px + 1 px spacing
    int x = (12 - textWidth) / 2;
    matrix.text(message, x, 1);
    matrix.endDraw();
}