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

bool          debugMode           = true;
int           currentHumidity     = 0;
float         currentPressure     = 0.0;
float         currentTemperature  = 0.0;
float         targetTemperature   = 0.0;
int           targetHumidity      = 0;

// Temperature in Celsius
float         minTemperature      = 0.0;    
float         maxTemperature      = 100.0;
float         minHumidity         = 0.0;
float         maxHumidity         = 100.0;

const int     temperatureRelayPin   = 16;
const int     humidityVentServoPin  = 37;
const int     trayServoPin          = 38;

const int     DHTSensorPin          = 39;
const int     BMESdaPin             = 33;
const int     BMESclPin             = 34;

const int     servoOpenPosition     = 0;
const int     servoClosedPosition   = 200;

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
  #define           DHTPIN          DHTSensorPin
  DHT dht(DHTPIN, DHTTYPE);
#elif   defined(SENSOR_BME280)
  #define           BME_SDA     	  BMESdaPin
  #define           BME_SCL         BMESclPin
  Adafruit_BME280   bme;
  TwoWire           I2CBME        = TwoWire(0);
#endif

#define       relayOn                    LOW
#define       relayOff                   HIGH 

AsyncWebServer  server(80);
Servo           ventServo;
Servo           trayServo;
TFT_eSPI        tft               = TFT_eSPI();
int16_t         displayHeight     = 128;
int16_t         displayWidth      = 160;

// Define colors
#define       BLACK                 0x0000
#define       WHITE                 0xFFFF
#define       RED                   0xF800

#define       WIFI_SSID             "ratinho_do_malandro"
#define       WIFI_PASSWORD         "newgerryforever2018"

void    initializeStorage();
void    controlTemperatureRelay(float currentTemperature, float targetTemperature);
void    controlHumidityVentServo(int currentHumidity, int targetHumidity);
void    saveTemperatureHumidityData(float currentTemperature, int currentHumidity);
void    saveIncubatorStatus(bool isIncubatorActive);
bool    getIncubatorStatus();
void    initializeWebServer();
void    connectToWifi();
void    connectServos();
void    initializeTFTDisplay();
void    updateTFTDisplay();
void    displayError(const String &errorMessage, const String &errorCode = "");
void    runIncubator();
void    initializeSensors();
void    displayLineOnTFT(uint16_t x, uint16_t y, const char* label, float value, const char* unit);
void    setupPIDControllers();
void    controlTrayServo();
String  readFileContent(const char *fileName);
void    writeContentToFile(const char *fileName, const String &content);
void    handleRootRequest(AsyncWebServerRequest *request);
void    handleTemperatureHumiditySettingsUpdate(AsyncWebServerRequest *request);
void    handleIncubatorStatusToggle(AsyncWebServerRequest *request);
void    handleDataFetchRequest(AsyncWebServerRequest *request);
void    handleSensorDataRequest(AsyncWebServerRequest *request);
void    printDebugMessage(String message);
void    handleServoSettingsUpdate(AsyncWebServerRequest *request);

void setup() {
  Serial.begin(115200);
  initializeTFTDisplay();
  initializeStorage();
  connectToWifi();
  initializeWebServer();
  AsyncElegantOTA.begin(&server);
  connectServos();
  initializeSensors();
  tft.fillScreen(BLACK);
  setupPIDControllers();
  printDebugMessage("Setup complete");
  delay(1000);
}

void loop() {
  bool        isIncubatorActive = getIncubatorStatus();
  Serial.println("Set status: " + String(isIncubatorActive ? "relayOn" : "relayOff"));
  targetTemperature             = readFileContent("/set_temp.txt").toFloat();
  targetHumidity                = readFileContent("/set_hum.txt").toInt();
  if (isIncubatorActive) {
    runIncubator();
  }
}

void writeContentToFile(const char *fileName, const String &content) {
  fs::File file = SPIFFS.open(fileName, "w");
  if (!file) {
    Serial.println(String("Error opening ") + fileName + " for writing");
    return;
  }
  file.print(content);
  file.close();
}

void handleServoSettingsUpdate(AsyncWebServerRequest *request) {
  String angle = request->getParam("angle")->value();
  String interval = request->getParam("interval")->value();
  Serial.println("Received updateServoSettings request with angle: " + angle + " and interval: " + interval);
  writeContentToFile("/servoTurnAngle.txt", angle);
  writeContentToFile("/interval.txt", interval);
  request->send(200, "text/plain", "OK");
}

void initializeTFTDisplay() {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(BLACK);
  tft.setTextSize(2);
  tft.setTextColor(WHITE, BLACK);
  tft.setCursor(0, 0);
  tft.print("INITIALISING TFT...");
  }
String readFileContent(const char *fileName) {
  if (!SPIFFS.exists(fileName)) {
    Serial.println(String("Error opening ") + fileName + " for reading");
    return "";
  }
  fs::File file = SPIFFS.open(fileName, "r");
  String content = file.readString();
  file.close();
  return content;
}

void saveTemperatureHumidityData(float currentTemperature, int currentHumidity) {
  fs::File file = SPIFFS.open("/data.txt", "a");
  
  if (!file) {
    Serial.println("Error opening data.txt for writing");
    displayError("091");
    return;
  }
  file.println(String(currentTemperature) + "," + String(currentHumidity) + "   ");
  file.close();
  Serial.println("Data saved to SPIFFS");
}

void saveIncubatorStatus(bool isIncubatorActive) {
  writeContentToFile("/set_status.txt", isIncubatorActive ? "1" : "0");
}

bool getIncubatorStatus() {
  return readFileContent("/set_status.txt").toInt() == 1;
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

// Initialize the sensor based on the selected type
void initializeSensors() {
  #ifdef SENSOR_DHT11
    dht.begin();
  #elif defined(SENSOR_BME280)
    I2CBME.begin(BME_SDA, BME_SCL);
    
    if (!bme.begin(0x77, &I2CBME)) {
      printDebugMessage("Could not find a valid BME280 sensor, check wiring!");
      while (1) {}
    }
  #endif
}

void controlTrayServo() {
  static unsigned long lastTurnTime = 0;
  static bool servoDirection = true;

  int interval, servoTurnAngle;
  interval = readFileContent("/interval.txt").toFloat();
  servoTurnAngle = readFileContent("/servoTurnAngle.txt").toFloat();

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
  printDebugMessage("Servo moved to position: " + String(newPosition) + (servoDirection ? " (forward)" : " (backward)"));
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

  #ifdef SENSOR_DHT11
    currentTemperature    = dht.readTemperature();
    currentHumidity       = dht.readHumidity();
  #elif defined(SENSOR_BME280)
    currentTemperature    = bme.readTemperature();
    currentHumidity       = bme.readHumidity();
    currentPressure        = bme.readPressure() / 100.0F;
  #endif


  if (isnan(currentHumidity) || isnan(currentTemperature)) {
    printDebugMessage("Failed to read from DHT sensor!");
    return;
  }

  if (currentTemperature < minTemperature || currentTemperature > maxTemperature || currentHumidity < minHumidity || currentHumidity > maxHumidity) {
    Serial.println("Sensor values out of range!");
    return;
  }

  saveTemperatureHumidityData(currentTemperature, currentHumidity);
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

void connectToWifi(){
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

void connectServos() {
  pinMode(temperatureRelayPin, OUTPUT);
  ventServo.attach(humidityVentServoPin);
  ventServo.write(200);
  trayServo.attach(trayServoPin);
  trayServo.write(200);
  }

void handleRootRequest(AsyncWebServerRequest *request) {
  request->send(SPIFFS, "/index.html", "text/html");
}

void handleTemperatureHumiditySettingsUpdate(AsyncWebServerRequest *request) {
  String temp = request->getParam("temp")->value();
  String hum = request->getParam("hum")->value();
  targetTemperature = temp.toFloat();
  targetHumidity = hum.toInt();

  Serial.println("Received updateSettings request with temp: " + temp + " and hum: " + hum);

  writeContentToFile("/set_temp.txt", String(targetTemperature));
  writeContentToFile("/set_hum.txt", String(targetHumidity));

  request->send(200, "text/plain", "OK");
}

void handleIncubatorStatusToggle(AsyncWebServerRequest *request) {
  bool currentStatus = getIncubatorStatus();
  saveIncubatorStatus(!currentStatus);
  String jsonResponse = "{\"status\": " + String(!currentStatus ? "true" : "false") + "}";
  Serial.println("Toggled incubator status to: " + String(!currentStatus ? "relayOn" : "relayOff"));
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
    printDebugMessage("An Error has occurred while mounting SPIFFS");
    displayError("Error mounting SPIFFS");
    return;
  }
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.println("SPIFFS mounted successfully!");
  printDebugMessage("SPIFFS mounted successfully!");
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

void printDebugMessage(String message) {
  if (debugMode) {
    Serial.println(message);
  }
}