// Fabfarm Egg Incubator
// Written by: Lucio

#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include <WiFiManager.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <math.h>
#include <ESP32Servo.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <ESPAsyncWebServer.h>
#include "NotoSansBold15.h"
#include <AsyncElegantOTA.h>// For over-the-air updates


bool      debugMode             = true;
int       currentHumidity       = 0;
float     currentPressure       = 0.0;
float     currentTemperature    = 0.0;
float     targetTemperature     = 0.0;
int       targetHumidity        = 0;
float     trayServoTurnInterval = 0;
float     trayServoTurnAngle    = 0;

// Temperature in Celsius
float     minTemperature        = 0.0;    
float     maxTemperature        = 100.0;
float     minHumidity           = 0.0;
float     maxHumidity           = 100.0;

const int mosfetPin             = 16;
const int humidityVentServoPin  = 37;
const int trayServoPin          = 38;

const int BMESdaPin             = 33;   //Board SDA to sensor SDI
const int BMESclPin             = 34;   //Board SCL to sensor SCK

const int servoOpenPosition     = 0;
const int servoClosedPosition   = 200;

#define           BME_SDA     	BMESdaPin
#define           BME_SCL       BMESclPin
Adafruit_BME280   bme;
TwoWire           I2CBME        = TwoWire(0);

#define       ON                  HIGH
#define       OFF                 LOW 

#define       BLACK            0x0000
#define       WHITE            0xFFFF
#define       RED              0xF800

#define       notoFont            NotoSansBold15
#define       hysteresis       0.2 // degrees Celsius

// select which pin will trigger the configuration portal when set to LOW
#define TRIGGER_PIN 0
int timeout = 120;


AsyncWebServer  server(80);
Servo           ventServo;
Servo           trayServo;
TFT_eSPI        tft              = TFT_eSPI();
int16_t         displayHeight    = 128;
int16_t         displayWidth     = 160;

void    initializeStorage();
void    controlHeatElementMosfet(float currentTemperature, float targetTemperature);
void    controlHumidityVentServo(int currentHumidity, int targetHumidity);
bool    getIncubatorStatus();
void    initializeWebServer();
void    connectServos();
void    initializeTFTDisplay();
void    updateTFTDisplay();
void    displayError(const String &errorMessage, const String &errorCode = "");
void    runIncubator();
void    initializeSensors();
void    displayLineOnTFT(uint16_t x, uint16_t y, const char* label, float value, const char* unit);
void    controlTrayServo();
String  readFromFile(const char *fileName);
void    writeToFile(const char *fileName, const String &content, bool append = false);
void    handleRootRequest(AsyncWebServerRequest *request);
void    handleTemperatureHumiditySettingsUpdate(AsyncWebServerRequest *request);
void    handleIncubatorStatusToggle(AsyncWebServerRequest *request);
void    handleDataFetchRequest(AsyncWebServerRequest *request);
void    handleSensorDataRequest(AsyncWebServerRequest *request);
void    handleCurrentSettingsRequest(AsyncWebServerRequest *request);
void    debugMessage(String message);
void    handleServoSettingsUpdate(AsyncWebServerRequest *request);
void    handleCurrentServoSettingsRequest(AsyncWebServerRequest *request);
void    wifiManagerSetup();
void    loadSettings();

void setup() {
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  wifiManagerSetup();
  AsyncElegantOTA.begin(&server);
  Serial.begin(115200);
  initializeTFTDisplay();
  initializeStorage();
  loadSettings();
  initializeWebServer();
  connectServos();
  initializeSensors();
  debugMessage("Setup complete");
  pinMode(mosfetPin, OUTPUT);
}

void loop() {
  if ( digitalRead(TRIGGER_PIN) == LOW) {
    WiFiManager wm;   
    wm.setConfigPortalTimeout(timeout);
    if (!wm.startConfigPortal("OnDemandAP")) {
      debugMessage("Failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.restart();
      delay(5000);
    }
    //if you get here you have connected to the WiFi
    debugMessage("Connected to WiFi");
  }
  bool isIncubatorActive = getIncubatorStatus();
  debugMessage("Incubator is: " + String(isIncubatorActive ? "active" : "not active"));
  runIncubator();
}

void loadSettings() {
  targetTemperature = readFromFile("/set_temp.txt").toFloat();
  targetHumidity = readFromFile("/set_hum.txt").toInt();
  trayServoTurnAngle = readFromFile("/trayServoTurnAngle.txt").toFloat();
  trayServoTurnInterval = readFromFile("/interval.txt").toFloat();
}

void wifiManagerSetup() {
  WiFiManager wm;
  // reset settings - wipe stored credentials for testing
  // these are stored by the esp library
  // wm.resetSettings();
  if (!wm.autoConnect("AutoConnectAP")) {
    debugMessage("Failed to connect");
    ESP.restart();
  }
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  debugMessage("Connected to WiFi");
}

void writeToFile(const char *fileName, const String &content, bool append) {
  fs::File file = SPIFFS.open(fileName, append ? "a" : "w");
  if (!file) {
    debugMessage(String("Error opening ") + fileName + " for writing");
    return;
  }
  file.print(content);
  file.close();
}

void initializeTFTDisplay() {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(BLACK);
  tft.loadFont(notoFont);
  tft.setTextSize(2);
  tft.setTextColor(WHITE, BLACK);
  tft.setCursor(0, 0);
  tft.print("INITIALISING TFT...");
  }

String readFromFile(const char *fileName) {
  if (!SPIFFS.exists(fileName)) {
    debugMessage(String("Error opening ") + fileName + " for reading");
    return "";
  }
  fs::File file = SPIFFS.open(fileName, "r");
  String content = file.readString();
  file.close();
  return content;
}

bool getIncubatorStatus() {
  return readFromFile("/set_status.txt").toInt() == 1;
}

void errorWithCode(String errorCode) {
  tft.fillScreen(BLACK);
  tft.setCursor(0, 0);
  tft.print("PLS CHECK DOCS:");
  tft.setCursor(0, 20);
  tft.print("ERROR CODE: " + errorCode);
}

void initializeSensors() {
  I2CBME.begin(BME_SDA, BME_SCL); 
  if (!bme.begin(0x77, &I2CBME)) {
    debugMessage("Could not find a valid BME280 sensor, check wiring!");
    while (1) {}
  }
}

void controlTrayServo() {
  static unsigned long lastTurnTime = 0;
  static bool trayServoDirection = true;

  int trayServoTurnInterval, trayServoTurnAngle;
  trayServoTurnInterval = readFromFile("/interval.txt").toFloat();
  trayServoTurnAngle = readFromFile("/trayServoTurnAngle.txt").toFloat();

  if (millis() - lastTurnTime >= trayServoTurnInterval) {
    int currentServoPosition = trayServo.read();
    int trayServoNewPosition;

    if (trayServoDirection) {
      trayServoNewPosition = currentServoPosition + trayServoTurnAngle;
    } else {
      trayServoNewPosition = currentServoPosition - trayServoTurnAngle;
    }
    trayServo.write(trayServoNewPosition);
    lastTurnTime = millis();
    trayServoDirection = !trayServoDirection; // Toggle direction
  debugMessage("Servo moved to position: " + String(trayServoNewPosition) + (trayServoDirection ? " (forward)" : " (backward)"));
  }
}

void runIncubator() {
  bool isIncubatorActive  = getIncubatorStatus();
  if (!isIncubatorActive) {
    tft.fillScreen(BLACK);
    tft.setCursor(0, 0);
    tft.print("SYSTEM PAUSED");
    digitalWrite(mosfetPin, OFF);
    ventServo.write(servoClosedPosition);
    return;
  }
  currentTemperature      = bme.readTemperature();
  currentHumidity         = bme.readHumidity();
  currentPressure         = bme.readPressure() / 100.0F;
  if (isnan(currentHumidity) || isnan(currentTemperature)) {
    debugMessage("Failed to read from sensor!");
    return;
  }
  if (currentTemperature < minTemperature || currentTemperature > maxTemperature || currentHumidity < minHumidity || currentHumidity > maxHumidity) {
    debugMessage("Sensor values out of range!");
    return;
  }
  writeToFile("/data.txt", String(currentTemperature) + "," + String(currentHumidity) + "   ", true);
  // debugMessage("Data saved to SPIFFS");
  controlHeatElementMosfet(currentTemperature, targetTemperature);
  controlHumidityVentServo(currentHumidity, targetHumidity);
  controlTrayServo();
  updateTFTDisplay();
}

void displayLineOnTFT(uint16_t x, uint16_t y, const char* label, float value, const char* unit) {
  tft.setCursor(x, y);
  tft.print(label);
  tft.printf("%.1f", value);
  tft.print(unit);
}

void updateTFTDisplay() {
  if (targetTemperature != -500 || targetHumidity != -500) {
    static float lastTemperature = -501.0, lastHumidity = -501.0, lastPressure = -501.0;

    if (currentTemperature != lastTemperature) {
      tft.fillRect(0, 0, tft.width(), 20, BLACK); // Clear the entire line
      displayLineOnTFT(0, 0, "T: ", currentTemperature, "C");
      lastTemperature = currentTemperature;
    }

    if (currentHumidity != lastHumidity) {
      tft.fillRect(0, 20, tft.width(), 20, BLACK); // Clear the entire line
      displayLineOnTFT(0, 20, "H: ", currentHumidity, "%");
      lastHumidity = currentHumidity;
    }

    if (currentPressure != lastPressure) {
      tft.fillRect(0, 40, tft.width(), 20, BLACK); // Clear the entire line
      displayLineOnTFT(0, 40, "P: ", currentPressure, "hPa");
      lastPressure = currentPressure;
    }

    tft.fillRect(0, 70, tft.width(), 20, BLACK); // Clear the entire line
    displayLineOnTFT(0, 70, "setT:", targetTemperature, "C");

    tft.fillRect(0, 90, tft.width(), 20, BLACK); // Clear the entire line
    displayLineOnTFT(0, 90, "setH:", targetHumidity, "%");
  }
}


void connectServos() {
  ventServo.attach(humidityVentServoPin);
  trayServo.attach(trayServoPin);
  }

void handleServoSettingsUpdate(AsyncWebServerRequest *request) {
  String trayServoTurnAngle      = request->getParam("angle")->value();
  String trayServoTurnInterval   = request->getParam("interval")->value();
  debugMessage("Received updateServoSettings request with angle: " + trayServoTurnAngle + " and interval: " + trayServoTurnInterval);
  writeToFile("/trayServoTurnAngle.txt", trayServoTurnAngle, false);
  writeToFile("/interval.txt", trayServoTurnInterval, false);
  request->send(200, "text/plain", "OK");
}
void handleCurrentSettingsRequest(AsyncWebServerRequest *request) {
  String json = "{\"temp\":" + String(targetTemperature) + ",\"hum\":" + String(targetHumidity) + "}";
  request->send(200, "application/json", json);
}
void handleCurrentServoSettingsRequest(AsyncWebServerRequest *request) {
  String json = "{\"angle\":" + String(trayServoTurnAngle) + ",\"interval\":" + String(trayServoTurnInterval) + "}";	
  request->send(200, "application/json", json);
}

void handleRootRequest(AsyncWebServerRequest *request) {
  request->send(SPIFFS, "/index.html", "text/html");
}

void handleTemperatureHumiditySettingsUpdate(AsyncWebServerRequest *request) {
  String temp = request->getParam("temp")->value();
  String hum = request->getParam("hum")->value();
  targetTemperature = temp.toFloat();
  targetHumidity = hum.toInt();
  debugMessage("Received updateSettings request with temp: " + temp + " and hum: " + hum);
  writeToFile("/set_temp.txt", String(targetTemperature), false);
  writeToFile("/set_hum.txt", String(targetHumidity), false);
  request->send(200, "text/plain", "OK");
}

void handleIncubatorStatusToggle(AsyncWebServerRequest *request) {
  bool currentStatus = getIncubatorStatus();
  writeToFile("/set_status.txt", currentStatus ? "0" : "1");
  String jsonResponse = "{\"status\": " + String(currentStatus ? "false" : "true") + "}";
  debugMessage("Toggled incubator status to: relay" + String(currentStatus ? "Off" : "On"));
  request->send(200, "application/json", jsonResponse);
}

void handleDataFetchRequest(AsyncWebServerRequest *request) {
  if (SPIFFS.exists("/data.txt")) {
    request->send(SPIFFS, "/data.txt", "text/plain");
  } else {
    request->send(404, "text/plain", "Data not found.");
  }
}

void handleSensorDataRequest(AsyncWebServerRequest *request) {
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  String jsonResponse = "{ \"temperature\": " + String(temperature) + ", \"humidity\": " + String(humidity) + " }";
  request->send(200, "application/json", jsonResponse);
}

void initializeWebServer() {
  server.on("/", HTTP_GET, handleRootRequest);
  server.on("/updateSettings", HTTP_GET, handleTemperatureHumiditySettingsUpdate);
  server.on("/toggleIncubator", HTTP_GET, handleIncubatorStatusToggle);
  server.on("/fetchData", HTTP_GET, handleDataFetchRequest);
  server.on("/getSensorData", HTTP_GET, handleSensorDataRequest);
  server.on("/updateServoSettings", HTTP_GET, handleServoSettingsUpdate);
  server.on("/getCurrentSettings", HTTP_GET, handleCurrentSettingsRequest);
  server.on("/getCurrentServoSettings", HTTP_GET, handleCurrentServoSettingsRequest);
  server.begin();
}

void controlHeatElementMosfet(float currentTemperature, float targetTemperature) {
  if (currentTemperature < targetTemperature - hysteresis) {
    digitalWrite(mosfetPin, ON);
  } else if (currentTemperature > targetTemperature + hysteresis) {
    digitalWrite(mosfetPin, OFF);
  }
}

void controlHumidityVentServo(int currentHumidity, int targetHumidity) {
  int humidityError = targetHumidity - currentHumidity;
  if (humidityError > 5) {
    // If the humidity is more than 5% less than the target, open the vent
    ventServo.write(servoOpenPosition);
  } else if (humidityError < -5) {
    // If the humidity is more than 5% greater than the target, close the vent
    ventServo.write(servoClosedPosition);
  } else {
    // If the humidity is within 5% of the target, do nothing
  }
}


void initializeStorage() {
  if (!SPIFFS.begin(true)) {
    debugMessage("An Error has occurred while mounting SPIFFS");
    displayError("Error mounting SPIFFS");
    return;
  }
  tft.setCursor(0, 0);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.println("SPIFFS mounted successfully!");
  debugMessage("SPIFFS mounted successfully!");
}

void displayError(const String &errorMessage, const String &errorCode) {
tft.fillScreen(RED);
tft.setTextColor(WHITE);
tft.setCursor(0, 0);
tft.print(errorMessage);
if (errorCode != "") {
tft.setCursor(0, 20);
tft.print("CODE: " + errorCode);
}
}

void debugMessage(String message) {
  if (debugMode) {
    Serial.println(message);
  }
}
//last time I checked we had 430 lines of code now 387