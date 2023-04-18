#include <Arduino.h>
#include <Wire.h>
#include <DHT.h> // Include DHT library instead of ClosedCube_HDC1080.h
#include <WiFi.h>
#include <math.h>
#include <ESP32Servo.h>
#include <AsyncElegantOTA.h>

#include <TFT_eSPI.h>// Hardware-specific library
#include <SPI.h>

#include <FS.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

#define BLACK 0x0000
#define WHITE 0xFFFF
#define GREY  0x5AEB

Servo myservo; // create a servo object

#define WIFI_SSID "ratinho_do_malandro"
#define WIFI_PASSWORD "newgerryforever2018"
const String deviceName = "fabfarmincubator";

const int relayPin = 16;
const String documentsPrefix = "incubators/" + deviceName;
const int servoPin = 37;

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

// These are used to get information about static SRAM and flash memory sizes
extern "C" char __data_start[];        // start of SRAM data
extern "C" char _end[];            // end of SRAM data (used to check amount of SRAM this program's variables use)
extern "C" char __data_load_end[];    // end of FLASH (used to check amount of Flash this program's code and data uses)

int16_t h = 128;
int16_t w = 160;

int dly = 10;

// Define DHT11 sensor type and pin
#define DHTTYPE DHT11
#define DHTPIN 39
DHT dht(DHTPIN, DHTTYPE);

int humSensor = 0;
int tempSensor = 0;
int tempDb = 0;
int humDb = 0;
bool wasNotFound = false;

#define TTL_Logic_Low
// Included option to use relays with TTL Logic LOW. Comment to use high
#ifdef TTL_Logic_Low
  #define ON   LOW
  #define OFF  HIGH
#else
  #define ON   HIGH
  #define OFF  LOW
#endif

// initialise each function
void setupStorage();
void errorLcd();
void relayControl(int tempSensor, int tempDb);
void servoControl(int humSensor, int humDb);
void sendToDatabase(int tempSensor, int humSensor);
void errorWithCode(String errorCode);
void syncIncubator(bool desiredStatus);
int tempFromDb();
int humFromDb();
void saveDesiredStatus(bool desiredStatus);
bool getDesiredStatus();
void setupWebServer();
void setupWifi();
void servoConect();
void initializeTft();
void updateDisplay();

void setup() {
  Serial.begin(9600);
  initializeTft();
  setupStorage();  // Setup storage for SPIFFS
  setupWifi();
  setupWebServer();
  AsyncElegantOTA.begin(&server); // Default values: username = "admin", password = "admin", port = 80, hostname = "elegant_ota", mdns_name = "elegant-ota", mdns_enabled = true
  delay(2000);
  servoConect();
  dht.begin(); // Initialize the DHT11 sensor
  tft.fillScreen(BLACK);
}

void loop() {
  bool desiredStatus = getDesiredStatus(); // Assuming the incubator should be active by default
  tempDb = tempFromDb();
  humDb = humFromDb();
  if (desiredStatus) {
    tempSensor = round(dht.readTemperature()); // Read temperature from DHT11 sensor
    humSensor = round(dht.readHumidity()); // Read humidity from DHT11 sensor
    relayControl(tempSensor, tempDb);
    servoControl(humSensor, humDb);
    updateDisplay();
    sendToDatabase(tempSensor, humSensor);
  
  } else {
    tft.fillScreen(BLACK);
    tft.setCursor(0, 0);
    tft.print("SYSTEM PAUSED");
    digitalWrite(relayPin, OFF); // HEATER OFF
    myservo.write(200);
  }
}

void updateDisplay() {
  if (tempDb != -500 || humDb != -500) {
      tft.setCursor(0, 0);
      tft.print("T: ");
      tft.print(tempSensor);
      tft.print("C");

      tft.setCursor(0, 20);
      tft.print("H: ");
      tft.print(humSensor);
      tft.print("%");

      tft.setCursor(0, 40);
      tft.print("set T: ");
      tft.print(tempDb);
      tft.print("C");

      tft.setCursor(0, 60);
      tft.print("set H: ");
      tft.print(humDb);
      tft.print("%");
  }
}

void initializeTft() {
  tft.init(); // Initialise the TFT screen
  tft.setRotation(1); // Rotate the screen 90 degrees counter clockwise
  tft.fillScreen(BLACK); // Fill the screen with white
  tft.setTextSize(2); // Set the font size to 2
  tft.setTextColor(WHITE, BLACK); // Set the font colour to grey on a black background
  tft.setCursor(0, 0);
  tft.print("TFT INITIALISING...");
  }

void setupWifi(){
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    }
  tft.fillScreen(BLACK); // Fill the screen with white
  tft.setCursor(0, 0);
  tft.print("Connected!");
  tft.print(WiFi.localIP());
  Serial.println();
  Serial.print("Connected!");
  Serial.println(WiFi.localIP());
}

void servoConect() {
  pinMode(relayPin, OUTPUT);
  myservo.attach(servoPin);
  myservo.write(200);
  }

void setupWebServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/updateSettings", HTTP_GET, [](AsyncWebServerRequest *request) {
    String temp = request->getParam("temp")->value();
    String hum = request->getParam("hum")->value();
    tempDb = temp.toInt();
    humDb = hum.toInt();

    Serial.println("Received updateSettings request with temp: " + temp + " and hum: " + hum);

    // Save the new desired temperature and humidity values to SPIFFS
    fs::File tempFile = SPIFFS.open("/desired_temp.txt", "w");
    if (tempFile) {
      tempFile.println(tempDb);
      tempFile.close();
    } else {
      Serial.println("Error opening desired_temp.txt for writing");
    }

    fs::File humFile = SPIFFS.open("/desired_hum.txt", "w");
    if (humFile) {
      humFile.println(humDb);
      humFile.close();
    } else {
      Serial.println("Error opening desired_hum.txt for writing");
    }

    request->send(200, "text/plain", "OK");
  });

  server.on("/toggleIncubator", HTTP_GET, [](AsyncWebServerRequest *request) {
    bool currentStatus = getDesiredStatus();
    saveDesiredStatus(!currentStatus);
    String jsonResponse = "{\"status\": " + String(!currentStatus ? "true" : "false") + "}";
    request->send(200, "application/json", jsonResponse);
  });

  server.on("/fetchData", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (SPIFFS.exists("/data.txt")) {
      request->send(SPIFFS, "/data.txt", "text/plain");
    } else {
      request->send(404, "text/plain", "Data not found.");
    }
  });

  server.begin();
}

void errorWithCode(String errorCode) {
  tft.fillScreen(BLACK);
  tft.setCursor(0, 0);
  tft.print("PLS CHECK DOCS:");
  tft.setCursor(0, 20);
  tft.print("ERROR CODE: " + errorCode);
}

void syncIncubator(bool desiredStatus) {
  saveDesiredStatus(desiredStatus);
}

void saveDesiredStatus(bool desiredStatus) {
  fs::File file = SPIFFS.open("/desired_status.txt", "w");
  
  if (!file) {
    Serial.println("Error opening desired_status.txt for writing");
    errorWithCode("071");
    return;
  }

  file.println(desiredStatus ? "1" : "0");
  file.close();
}

bool getDesiredStatus() {
  fs::File file = SPIFFS.open("/desired_status.txt", "r");
  
  if (!file) {
    Serial.println("Error opening desired_status.txt for reading");
    errorWithCode("071");
    return true; // Default to true if there is an error
  }

  bool desiredStatus = file.parseInt() == 1;
  file.close();

  return desiredStatus;
}

int tempFromDb() {
  // Read the desired temperature from SPIFFS
  fs::File file = SPIFFS.open("/desired_temp.txt", "r");
  if (!file) {
    Serial.println("Error opening file for reading");
    return -500;
  }
  int res = file.parseInt();
  file.close();
  return res;
}

int humFromDb() {
  // Read the desired humidity from SPIFFS
  fs::File file = SPIFFS.open("/desired_hum.txt", "r");
  if (!file) {
    Serial.println("Error opening file for reading");
    return -500;
  }
  int res = file.parseInt();
  file.close();
  return res;
}

void sendToDatabase(int tempSensor, int humSensor) {
  fs::File file = SPIFFS.open("/data.txt", "a");
  
  if (!file) {
    Serial.println("Error opening file for writing");
    errorWithCode("091");
    return;
  }

  file.println("///" + String(tempSensor) + "," + String(humSensor));
  file.close();

  Serial.println("Data saved to SPIFFS");
}


void relayControl(int tempSensor, int tempDb) {
  if (tempSensor >= tempDb) {
    digitalWrite(relayPin, OFF); // HEATER OFF
  } else if (tempSensor < tempDb) {
    digitalWrite(relayPin, ON); // HEATER ON
  }
}

void servoControl(int humSensor, int humDb) {
  if (humSensor > humDb) {
    myservo.write(0); // ventilation opens
  } else {
    myservo.write(200); // ventilation closes
  }
}

void errorLcd() { // when there's DB error
  tft.fillScreen(BLACK);
  tft.setCursor(0, 0);
  tft.print("DB ERROR");

  tft.setCursor(0, 20);
  tft.print("PLS CHECK LOG");
}

void setupStorage() {
  // Initialize SPIFFS (file system)
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
}

