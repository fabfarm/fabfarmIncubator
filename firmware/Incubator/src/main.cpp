// Fabfarm Egg Incubator
// Written by: Lucio

#include <Arduino.h>
#include <Wire.h>
#include <DHT.h>
#include <Adafruit_BME280.h>
#include <WiFi.h>
#include <math.h>
#include <ESP32Servo.h>
#include <AsyncElegantOTA.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <PID_v1.h>

bool          DEBUG_MODE          = true;
int           humSensor           = 0;
float         presSensor          = 0.0;
float         tempSensor          = 0.0;
float         tempDb              = 0.0;
int           humDb               = 0;

// Temperature in Celsius
float         tempMin             = 0.0;    
float         tempMax             = 100.0;
float         humMin              = 0.0;
float         humMax              = 100.0;

const int     relayPin            = 16;
const int     ventServoPin        = 37;
const int     trayServoPin        = 38;

const int     dhtPin              = 39;
const int     bmeSdaPin           = 33;
const int     bmeSclPin           = 34;

const int     SERVO_OPEN          = 0;
const int     SERVO_CLOSED        = 200;

// PID objects and tuning parameters for temperature and humidity
double        tempSetpoint, tempInput,  tempOutput;
double        humSetpoint,  humInput,   humOutput;
double        tempKp  = 2, tempKi   = 5, tempKd   = 1;
double        humKp   = 2, humKi    = 5, humKd    = 1;

PID tempPID(&tempInput, &tempOutput,  &tempSetpoint,  tempKp, tempKi, tempKd, DIRECT);
PID humPID(&humInput,   &humOutput,   &humSetpoint,   humKp,  humKi,  humKd,  DIRECT);



//Define sensor type and pin
//#define SENSOR_DHT11
#define             SENSOR_BME280
#ifdef              SENSOR_DHT11
  #define           DHTTYPE DHT11
  #define           DHTPIN          dhtPin
  DHT dht(DHTPIN, DHTTYPE);
#elif   defined(SENSOR_BME280)
  #define           BME_SDA     	  bmeSdaPin
  #define           BME_SCL         bmeSclPin
  Adafruit_BME280   bme;
  TwoWire           I2CBME        = TwoWire(0);
#endif

#define       ON                    LOW
#define       OFF                   HIGH 

// These are used to get information about static SRAM and flash memory sizes
extern "C" char __data_start[];             // start of SRAM data
extern "C" char _end[];                     // end of SRAM data (used to check amount of SRAM this program's variables use)
extern "C" char __data_load_end[];          // end of FLASH (used to check amount of Flash this program's code and data uses)

AsyncWebServer  server(80);
Servo           ventServo;
Servo           trayServo;
TFT_eSPI        tft               = TFT_eSPI();
int16_t         h                 = 128;
int16_t         w                 = 160;

// Define colors
#define       BLACK                 0x0000
#define       WHITE                 0xFFFF
#define       GREY                  0x5AEB
#define       RED                   0xF800
#define       GREEN                 0x07E0
#define       BLUE                  0x001F
#define       CYAN                  0x07FF
#define       MAGENTA               0xF81F
#define       YELLOW                0xFFE0
#define       ORANGE                0xFD20
#define       BROWN                 0x79E0

#define       WIFI_SSID             "ratinho_do_malandro"
#define       WIFI_PASSWORD         "newgerryforever2018"

void    setupStorage();
void    relayControl(float tempSensor, float tempDb);
void    ventServoControl(int humSensor, int humDb);
void    sendToDatabase(float tempSensor, int humSensor);
void    saveDesiredStatus(bool desiredStatus);
bool    getDesiredStatus();
void    setupWebServer();
void    setupWifi();
void    servoConect();
void    initializeTft();
void    updateDisplay();
void    displayError(const String &errorMessage, const String &errorCode = "");
void    incubatorRun();
void    initializeSensor();
void    printDisplayLine(uint16_t x, uint16_t y, const char* label, float value, const char* unit);
void    initializePID();
void    trayServoControl();
String  readFromFile(const char *fileName);
void    saveToFile(const char *fileName, const String &content);
void    handleRoot(AsyncWebServerRequest *request);
void    handleUpdateSettings(AsyncWebServerRequest *request);
void    handleToggleIncubator(AsyncWebServerRequest *request);
void    handleFetchData(AsyncWebServerRequest *request);
void    handleGetSensorData(AsyncWebServerRequest *request);
void    debugPrint(String message);
void    handleUpdateServoSettings(AsyncWebServerRequest *request);

void setup() {
  Serial.begin(115200);
  initializeTft();
  setupStorage();
  setupWifi();
  setupWebServer();
  AsyncElegantOTA.begin(&server);
  servoConect();
  initializeSensor();
  tft.fillScreen(BLACK);
  initializePID();
  debugPrint("Setup complete");
  delay(1000);
}

void loop() {
  bool        desiredStatus       = getDesiredStatus();
  Serial.println("Desired status: " + String(desiredStatus ? "ON" : "OFF"));
  tempDb                          = readFromFile("/desired_temp.txt").toFloat();
  humDb                           = readFromFile("/desired_hum.txt").toInt();
  if (desiredStatus) {
    incubatorRun();
  }
}

void handleUpdateServoSettings(AsyncWebServerRequest *request) {
  String angle = request->getParam("angle")->value();
  String interval = request->getParam("interval")->value();
  
  Serial.println("Received updateServoSettings request with angle: " + angle + " and interval: " + interval);

  saveToFile("/servoTurnAngle.txt", angle);
  saveToFile("/interval.txt", interval);

  request->send(200, "text/plain", "OK");
}

void initializeTft() {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(BLACK);
  tft.setTextSize(2);
  tft.setTextColor(WHITE, BLACK);
  tft.setCursor(0, 0);
  tft.print("INITIALISING TFT...");
  }
String readFromFile(const char *fileName) {
  if (!SPIFFS.exists(fileName)) {
    Serial.println(String("Error opening ") + fileName + " for reading");
    return "";
  }

  fs::File file = SPIFFS.open(fileName, "r");
  String content = file.readString();
  file.close();
  return content;
}

void saveToFile(const char *fileName, const String &content) {
  fs::File file = SPIFFS.open(fileName, "w");
  if (!file) {
    Serial.println(String("Error opening ") + fileName + " for writing");
    return;
  }
  file.print(content);
  file.close();
}

void saveDesiredStatus(bool desiredStatus) {
  saveToFile("/desired_status.txt", desiredStatus ? "1" : "0");
}

bool getDesiredStatus() {
  return readFromFile("/desired_status.txt").toInt() == 1;
}

void sendToDatabase(float tempSensor, int humSensor) {
  String data = "Temperature: " + String(tempSensor) + "C, Humidity: " + String(humSensor) + "%";
  saveToFile("/data.txt", data);
}

void initializePID() {
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

// Initialize the sensor based on the selected type
void initializeSensor() {
  #ifdef SENSOR_DHT11
    dht.begin();
  #elif defined(SENSOR_BME280)
    I2CBME.begin(BME_SDA, BME_SCL);
    
    if (!bme.begin(0x77, &I2CBME)) {
      debugPrint("Could not find a valid BME280 sensor, check wiring!");
      while (1) {}
    }
  #endif
}

void trayServoControl() {
  static unsigned long lastTurnTime = 0;
  static bool servoDirection = true;

  int interval, servoTurnAngle;
  interval = readFromFile("/interval.txt").toFloat();
  servoTurnAngle = readFromFile("/servoTurnAngle.txt").toFloat();

  if (millis() - lastTurnTime >= interval) {
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
  debugPrint("Servo moved to position: " + String(newPosition) + (servoDirection ? " (forward)" : " (backward)"));
  }
}


void incubatorRun() {
  bool        desiredStatus       = getDesiredStatus();
  if (!desiredStatus) {
    tft.fillScreen(BLACK);
    tft.setCursor(0, 0);
    tft.print("SYSTEM PAUSED");
    digitalWrite(relayPin, OFF);
    ventServo.write(200);
    return;
  }

  #ifdef SENSOR_DHT11
    tempSensor   = dht.readTemperature();
    humSensor    = dht.readHumidity();
  #elif defined(SENSOR_BME280)
    tempSensor   = bme.readTemperature();
    humSensor    = bme.readHumidity();
    presSensor   = bme.readPressure() / 100.0F;
  #endif


  if (isnan(humSensor) || isnan(tempSensor)) {
    debugPrint("Failed to read from DHT sensor!");
    return;
  }

  if (tempSensor < tempMin || tempSensor > tempMax || humSensor < humMin || humSensor > humMax) {
    Serial.println("Sensor values out of range!");
    return;
  }

  sendToDatabase(tempSensor, humSensor);
  relayControl(tempSensor, tempDb);
  ventServoControl(humSensor, humDb);
  trayServoControl();
  updateDisplay();
}

void printDisplayLine(uint16_t x, uint16_t y, const char* label, float value, const char* unit) {
  //tft.fillScreen(BLACK);
  tft.setCursor(x, y);
  tft.print(label);
  tft.printf("%.1f", value);
  tft.print(unit);
}

void updateDisplay() {
  if (tempDb != -500 || humDb != -500) {
    tft.fillScreen(BLACK);
    printDisplayLine(0, 0,    "T: ",    tempSensor,   "C");
    printDisplayLine(0, 20,   "H: ",    humSensor,    "%");
    printDisplayLine(0, 40,   "P: ",    presSensor, "hPa");
    printDisplayLine(0, 70,   "setT:",  tempDb,       "C");
    printDisplayLine(0, 90,   "setH:",  humDb,        "%");
  }
}

void setupWifi(){
  tft.fillScreen(BLACK);
  tft.setCursor(0, 0);
  tft.print("Connecting...");
  Serial.println();
  Serial.print("Connecting...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    }
  tft.fillScreen(BLACK);
  tft.setCursor(0, 0);
  tft.print("Connected!");
  tft.print(WiFi.localIP());
  Serial.println();
  Serial.print("Connected!");
  Serial.println(WiFi.localIP());
}

void servoConect() {
  pinMode(relayPin, OUTPUT);
  ventServo.attach(ventServoPin);
  ventServo.write(200);
  trayServo.attach(trayServoPin);
  trayServo.write(200);
  }

void handleRoot(AsyncWebServerRequest *request) {
  request->send(SPIFFS, "/index.html", "text/html");
}

void handleUpdateSettings(AsyncWebServerRequest *request) {
  String temp = request->getParam("temp")->value();
  String hum = request->getParam("hum")->value();
  tempDb = temp.toFloat();
  humDb = hum.toInt();

  Serial.println("Received updateSettings request with temp: " + temp + " and hum: " + hum);

  saveToFile("/desired_temp.txt", String(tempDb));
  saveToFile("/desired_hum.txt", String(humDb));

  request->send(200, "text/plain", "OK");
}

void handleToggleIncubator(AsyncWebServerRequest *request) {
  bool currentStatus = getDesiredStatus();
  saveDesiredStatus(!currentStatus);
  String jsonResponse = "{\"status\": " + String(!currentStatus ? "true" : "false") + "}";
  Serial.println("Toggled incubator status to: " + String(!currentStatus ? "ON" : "OFF"));
  request->send(200, "application/json", jsonResponse);
}


void handleFetchData(AsyncWebServerRequest *request) {
  if (SPIFFS.exists("/data.txt")) {
    request->send(SPIFFS, "/data.txt", "text/plain");
  } else {
    request->send(404, "text/plain", "Data not found.");
  }
}

void handleGetSensorData(AsyncWebServerRequest *request) {
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();

  String jsonResponse = "{ \"temperature\": " + String(temperature) + ", \"humidity\": " + String(humidity) + " }";
  request->send(200, "application/json", jsonResponse);
}

void setupWebServer() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/updateSettings", HTTP_GET, handleUpdateSettings);
  server.on("/toggleIncubator", HTTP_GET, handleToggleIncubator);
  server.on("/fetchData", HTTP_GET, handleFetchData);
  server.on("/getSensorData", HTTP_GET, handleGetSensorData);
  server.on("/updateServoSettings", HTTP_GET, handleUpdateServoSettings);
  server.begin();
}

void relayControl(float tempSensor, float tempDb) {
  tempInput = tempSensor;
  tempSetpoint = tempDb;
  tempPID.Compute();

  if (tempOutput > 0.5) {
    digitalWrite(relayPin, ON);
  } else {
    digitalWrite(relayPin, OFF);
  }
}

void ventServoControl(int humSensor, int humDb) {
  humInput = humSensor;
  humSetpoint = humDb;
  humPID.Compute();
  int servoPosition = SERVO_CLOSED - humOutput;
  ventServo.write(servoPosition);
}

void setupStorage() {
  if (!SPIFFS.begin(true)) {
    debugPrint("An Error has occurred while mounting SPIFFS");
    displayError("Error mounting SPIFFS");
    return;
  }
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.println("SPIFFS mounted successfully!");
  debugPrint("SPIFFS mounted successfully!");
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

void debugPrint(String message) {
  if (DEBUG_MODE) {
    Serial.println(message);
  }
}