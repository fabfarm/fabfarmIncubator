// Fabfarm Egg Incubator
// Written by: Lucio

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <WiFiManager.h>
#include <math.h>
#include <ESP32Servo.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <PID_v1.h>
#include "NotoSansBold15.h"

bool      debugMode           = true;
int       currentHumidity     = 0;
float     currentPressure     = 0.0;
float     currentTemperature  = 0.0;
float     targetTemperature   = 0.0;
int       targetHumidity      = 0;
float     servoTurnInterval            = 0;
float     servoTurnAngle               = 0;

// Temperature in Celsius
float     minTemperature      = 0.0;    
float     maxTemperature      = 100.0;
float     minHumidity         = 0.0;
float     maxHumidity         = 100.0;

const int temperatureRelayPin   = 16;
const int humidityVentServoPin  = 37;
const int trayServoPin          = 38;

const int BMESdaPin             = 33;   //Board SDA to sensor SDI
const int BMESclPin             = 34;   //Board SCL to sensor SCK

const int servoOpenPosition     = 0;
const int servoClosedPosition   = 200;

double    tempSetpoint, tempInput,  tempOutput;
double    humSetpoint,  humInput,   humOutput;
double    tempKp, tempKi, tempKd;
double    humKp, humKi, humKd;


PID tempPID(&tempInput, &tempOutput,  &tempSetpoint,  tempKp, tempKi, tempKd, DIRECT);
PID humPID(&humInput,   &humOutput,   &humSetpoint,   humKp,  humKi,  humKd,  DIRECT);

#define           BME_SDA     	BMESdaPin
#define           BME_SCL       BMESclPin
Adafruit_BME280   bme;
TwoWire           I2CBME      = TwoWire(0);

#define       relayOn           LOW
#define       relayOff          HIGH 

AsyncWebServer  server(80);
Servo           ventServo;
Servo           trayServo;
TFT_eSPI        tft           = TFT_eSPI();
int16_t         displayHeight = 128;
int16_t         displayWidth  = 160;

// Define colors
#define       BLACK            0x0000
#define       WHITE            0xFFFF
#define       RED              0xF800

#define       notoFont            NotoSansBold15


void    initializeStorage();
void    controlTemperatureRelay(float currentTemperature, float targetTemperature);
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
void    setupPIDControllers();
void    controlTrayServo();
String  readFromFile(const char *fileName);
void    writeToFile(const char *fileName, const String &content, bool append = false);
void    handleRootRequest(AsyncWebServerRequest *request);
void    handleTemperatureHumiditySettingsUpdate(AsyncWebServerRequest *request);
void    handleIncubatorStatusToggle(AsyncWebServerRequest *request);
void    handleDataFetchRequest(AsyncWebServerRequest *request);
void    handleSensorDataRequest(AsyncWebServerRequest *request);
void    handlePIDSettingsUpdate(AsyncWebServerRequest *request);
void    handleCurrentSettingsRequest(AsyncWebServerRequest *request);
void    debugMessage(String message);
void    handleServoSettingsUpdate(AsyncWebServerRequest *request);
void    handleCurrentServoSettingsRequest(AsyncWebServerRequest *request);
void    handleCurrentPIDSettingsRequest(AsyncWebServerRequest *request);
void    wifiManagerSetup();
void    loadSettings();

void setup() {
  Serial.begin(115200);
  wifiManagerSetup();
  initializeTFTDisplay();
  initializeStorage();
  loadSettings();
  initializeWebServer();
  connectServos();
  initializeSensors();
  setupPIDControllers();
  debugMessage("Setup complete");
}

void loop() {
  bool isIncubatorActive = getIncubatorStatus();
  debugMessage("Incubator is: " + String(isIncubatorActive ? "active" : "not active"));
  runIncubator();
}

void loadSettings() {
  tempKp = readFromFile("/tempKp.txt").toDouble();
  tempKi = readFromFile("/tempKi.txt").toDouble();
  tempKd = readFromFile("/tempKd.txt").toDouble();
  humKp = readFromFile("/humKp.txt").toDouble();
  humKi = readFromFile("/humKi.txt").toDouble();
  humKd = readFromFile("/humKd.txt").toDouble();
  targetTemperature = readFromFile("/set_temp.txt").toFloat();
  targetHumidity = readFromFile("/set_hum.txt").toInt();
  servoTurnAngle = readFromFile("/servoTurnAngle.txt").toFloat();
  servoTurnInterval = readFromFile("/interval.txt").toFloat();
}

void wifiManagerSetup() {
WiFiManager wm;
if (!wm.autoConnect("AutoConnectAP")) {
debugMessage("Failed to connect");
ESP.restart();
}
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

void setupPIDControllers() {
  tempPID.SetMode(AUTOMATIC);
  tempPID.SetOutputLimits(0, 1);
  humPID.SetMode(AUTOMATIC);
  humPID.SetOutputLimits(0, 180);
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
  static bool servoDirection = true;

  int servoTurnInterval, servoTurnAngle;
  servoTurnInterval = readFromFile("/interval.txt").toFloat();
  servoTurnAngle = readFromFile("/servoTurnAngle.txt").toFloat();

  if (millis() - lastTurnTime >= servoTurnInterval) {
    int currentServoPosition = trayServo.read();
    int newPosition;

    if (servoDirection) {
      newPosition = currentServoPosition + servoTurnAngle;
    } else {
      newPosition = currentServoPosition - servoTurnAngle;
    }

    trayServo.write(newPosition);
    lastTurnTime = millis();
    servoDirection = !servoDirection; // Toggle direction
  debugMessage("Servo moved to position: " + String(newPosition) + (servoDirection ? " (forward)" : " (backward)"));
  }
}

void runIncubator() {
  bool        isIncubatorActive   = getIncubatorStatus();
  if (!isIncubatorActive) {
    tft.fillScreen(BLACK);
    tft.setCursor(0, 0);
    tft.print("SYSTEM PAUSED");
    digitalWrite(temperatureRelayPin, relayOff);
    ventServo.write(200);
    return;
  }

  currentTemperature    = bme.readTemperature();
  currentHumidity       = bme.readHumidity();
  currentPressure        = bme.readPressure() / 100.0F;

  if (isnan(currentHumidity) || isnan(currentTemperature)) {
    debugMessage("Failed to read from sensor!");
    return;
  }

  if (currentTemperature < minTemperature || currentTemperature > maxTemperature || currentHumidity < minHumidity || currentHumidity > maxHumidity) {
    debugMessage("Sensor values out of range!");
    return;
  }

  writeToFile("/data.txt", String(currentTemperature) + "," + String(currentHumidity) + "   ", true);
  debugMessage("Data saved to SPIFFS");
  controlTemperatureRelay(currentTemperature, targetTemperature);
  controlHumidityVentServo(currentHumidity, targetHumidity);
  controlTrayServo();
  updateTFTDisplay();
}

void displayLineOnTFT(uint16_t x, uint16_t y, const char* label, float value, const char* unit) {
  //tft.fillScreen(BLACK);
  tft.setCursor(x, y);
  tft.print(label);
  tft.printf("%.1f", value);
  tft.print(unit);
}

void updateTFTDisplay() {
  if (targetTemperature != -500 || targetHumidity != -500) {
    tft.fillScreen(BLACK);
    displayLineOnTFT(0, 0,    "T: ",    currentTemperature, "C");
    displayLineOnTFT(0, 20,   "H: ",    currentHumidity,    "%");
    displayLineOnTFT(0, 40,   "P: ",    currentPressure,     "hPa");
    displayLineOnTFT(0, 70,   "setT:",  targetTemperature,  "C");
    displayLineOnTFT(0, 90,   "setH:",  targetHumidity,     "%");
  }
}

void connectServos() {
  pinMode(temperatureRelayPin, OUTPUT);
  ventServo.attach(humidityVentServoPin);
  ventServo.write(200);
  trayServo.attach(trayServoPin);
  trayServo.write(200);
  }

void handleServoSettingsUpdate(AsyncWebServerRequest *request) {
  String servoTurnAngle      = request->getParam("angle")->value();
  String servoTurnInterval   = request->getParam("interval")->value();
  debugMessage("Received updateServoSettings request with angle: " + servoTurnAngle + " and interval: " + servoTurnInterval);
  writeToFile("/servoTurnAngle.txt", servoTurnAngle, false);
  writeToFile("/interval.txt", servoTurnInterval, false);
  request->send(200, "text/plain", "OK");
}

void handleCurrentSettingsRequest(AsyncWebServerRequest *request) {
  String json = "{\"temp\":" + String(targetTemperature) + ",\"hum\":" + String(targetHumidity) + "}";
  request->send(200, "application/json", json);
}
void handleCurrentServoSettingsRequest(AsyncWebServerRequest *request) {
  String json = "{\"angle\":" + String(servoTurnAngle) + ",\"interval\":" + String(servoTurnInterval) + "}";	
  request->send(200, "application/json", json);
}
void handleCurrentPIDSettingsRequest(AsyncWebServerRequest *request) {
  String json = "{\"tempKp\":" + String(tempKp) + ",\"tempKi\":" + String(tempKi) + ",\"tempKd\":" + String(tempKd) + ",\"humKp\":" + String(humKp) + ",\"humKi\":" + String(humKi) + ",\"humKd\":" + String(humKd) + "}";
  request->send(200, "application/json", json);
}

void handlePIDSettingsUpdate(AsyncWebServerRequest *request) {
  String tempKp     = request->getParam("tempKp")->value();
  String tempKi     = request->getParam("tempKi")->value();
  String tempKd     = request->getParam("tempKd")->value();
  String humKp      = request->getParam("humKp")->value();
  String humKi      = request->getParam("humKi")->value();
  String humKd      = request->getParam("humKd")->value();
  debugMessage("Received updatePIDSettings request with tempKp: " + tempKp + " tempKi: " + tempKi + " tempKd: " + tempKd + " humKp: " + humKp + " humKi: " + humKi + " humKd: " + humKd);
  writeToFile("/tempKp.txt", tempKp, false);
  writeToFile("/tempKi.txt", tempKi, false);
  writeToFile("/tempKd.txt", tempKd, false);
  writeToFile("/humKp.txt", humKp, false);
  writeToFile("/humKi.txt", humKi, false);
  writeToFile("/humKd.txt", humKd, false);
  request->send(200, "text/plain", "OK");
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
  server.on("/updatePIDSettings", HTTP_GET, handlePIDSettingsUpdate);
  server.on("/getCurrentSettings", HTTP_GET, handleCurrentSettingsRequest);
  server.on("/getCurrentServoSettings", HTTP_GET, handleCurrentServoSettingsRequest);
  server.on("/getCurrentPIDSettings", HTTP_GET, handleCurrentPIDSettingsRequest);

  server.begin();
}
void controlTemperatureRelay(float currentTemperature, float targetTemperature) {
  tempInput = currentTemperature;
  tempSetpoint = targetTemperature;
  tempPID.Compute();

  if (tempOutput > 0.5) {
    digitalWrite(temperatureRelayPin, relayOn);
  } else {
    digitalWrite(temperatureRelayPin, relayOff);
  }
}

void controlHumidityVentServo(int currentHumidity, int targetHumidity) {
  humInput = currentHumidity;
  humSetpoint = targetHumidity;
  humPID.Compute();
  int servoPosition = servoClosedPosition - humOutput;
  ventServo.write(servoPosition);
}

void initializeStorage() {
  if (!SPIFFS.begin(true)) {
    debugMessage("An Error has occurred while mounting SPIFFS");
    displayError("Error mounting SPIFFS");
    return;
  }
  tft.fillScreen(TFT_BLACK);
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
//last time I checked we had 410 lines of code now 430