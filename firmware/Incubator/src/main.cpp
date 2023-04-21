// Fabfarm Egg Incubator
// Written by: Lucio

#include <Arduino.h>                        // Arduino framework
#include <Wire.h>                           // I2C library
#include <DHT.h>                            // DHT library
#include <Adafruit_BME280.h>                // BME280 library
#include <WiFi.h>                           // WiFi library
#include <math.h>                           // Math library
#include <ESP32Servo.h>                     // Servo library
#include <AsyncElegantOTA.h>                // OTA library
#include <TFT_eSPI.h>                       // TFT library
#include <SPI.h>                            // SPI library
#include <FS.h>                             // SPIFFS library
#include <SPIFFS.h>                         // SPIFFS library
#include <ESPAsyncWebServer.h>              // Web server library

int           humSensor           = 0;
float         presSensor          = 0.0;
float         tempSensor          = 0.0;
float         tempDb              = 0.0;
int           humDb               = 0;

// Define the range of acceptable values
float         tempMin             = 0.0;    // minimum temperature in Celsius (change as needed)
float         tempMax             = 100.0;  // maximum temperature in Celsius (change as needed)
float         humMin              = 0.0;    // minimum humidity in percent (change as needed)
float         humMax              = 100.0;  // maximum humidity in percent (change as needed)

// PIN definitions / assignments
const int     relayPin            = 16;
const int     servoPin            = 37;

// Sensor pins
const int     dhtPin              = 39;
const int     bmeSdaPin           = 33;
const int     bmeSclPin           = 34;

// Constants for servo positions
const int     SERVO_OPEN          = 0;
const int     SERVO_CLOSED        = 200;


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
Servo           myservo;                          // create a servo object
TFT_eSPI        tft               = TFT_eSPI();   // Invoke custom library
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

// Define fonts
#define       WIFI_SSID             "fabfarm_ele_container"
#define       WIFI_PASSWORD         "imakestuff"

// initialise each function
void    setupStorage();
void    relayControl(float tempSensor, float tempDb);
void    servoControl(int humSensor, int humDb);
void    sendToDatabase(float tempSensor, int humSensor);
float   desiredTempFromDb();
int     desiredHumFromDb();
void    saveDesiredStatus(bool desiredStatus);
bool    getDesiredStatus();
void    setupWebServer();
void    setupWifi();
void    servoConect();
void    initializeTft();
void    updateDisplay();
float   roundToOneDecimal(float value);
void    displayError(const String &errorMessage, const String &errorCode = "");
void    incubatorRun();
void    initializeSensor();
void    printDisplayLine(uint16_t x, uint16_t y, const char* label, float value, const char* unit);

void setup() {
  Serial.begin(9600);
  initializeTft();
  setupStorage();                           // Setup storage for SPIFFS
  setupWifi();
  setupWebServer();
  AsyncElegantOTA.begin(&server);           // Default values: username = "admin", password = "admin", port = 80, hostname = "elegant_ota", mdns_name = "elegant-ota", mdns_enabled = true
  servoConect();
  initializeSensor();
  tft.fillScreen(BLACK);
}

//############################################################################################################
// loop function
//############################################################################################################

void loop() {
  bool        desiredStatus       = getDesiredStatus();
  tempDb                          = desiredTempFromDb();
  humDb                           = desiredHumFromDb();
  // incubator function calls
  incubatorRun();
}

//############################################################################################################
// Functions
//############################################################################################################

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


// Initialize the sensor based on the selected type
void initializeSensor() {
  #ifdef SENSOR_DHT11
    dht.begin();
  #elif defined(SENSOR_BME280)
    I2CBME.begin(BME_SDA, BME_SCL);
    
    if (!bme.begin(0x77, &I2CBME)) {
      Serial.println("Could not find a valid BME280 sensor, check wiring!");
      while (1) {}
    }
  #endif
}

// incubator function
void incubatorRun() {
  bool        desiredStatus       = getDesiredStatus();
  if (!desiredStatus) {
    tft.fillScreen(BLACK);
    tft.setCursor(0, 0);
    tft.print("SYSTEM PAUSED");
    digitalWrite(relayPin, OFF);
    myservo.write(200);
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
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  if (tempSensor < tempMin || tempSensor > tempMax || humSensor < humMin || humSensor > humMax) {
    Serial.println("Sensor values out of range!");
    return;
  }

  sendToDatabase(tempSensor, humSensor);
  relayControl(tempSensor, tempDb);
  servoControl(humSensor, humDb);
  updateDisplay();
}

float roundToOneDecimal(float value) {
  return round(value * 10.0) / 10.0;
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

void initializeTft() {
  tft.init(); // Initialise the TFT screen
  tft.setRotation(1); // Rotate the screen 90 degrees counter clockwise
  tft.fillScreen(BLACK); // Fill the screen with white
  tft.setTextSize(2); // Set the font size to 2
  tft.setTextColor(WHITE, BLACK); // Set the font colour to grey on a black background
  tft.setCursor(0, 0);
  tft.print("INITIALISING TFT...");
  }

void setupWifi(){
  tft.fillScreen(BLACK); // Fill the screen with white
  tft.setCursor(0, 0);
  tft.print("Connecting...");
  Serial.println();
  Serial.print("Connecting...");
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

// Set up the web server routes and start the server
void setupWebServer() {
  // Serve the index.html file
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  // Update settings
  server.on("/updateSettings", HTTP_GET, [](AsyncWebServerRequest *request) {
    String temp = request->getParam("temp")->value();
    String hum = request->getParam("hum")->value();
    tempDb = temp.toFloat();
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

  // Toggle incubator status
  server.on("/toggleIncubator", HTTP_GET, [](AsyncWebServerRequest *request) {
    bool currentStatus = getDesiredStatus();
    saveDesiredStatus(!currentStatus);
    String jsonResponse = "{\"status\": " + String(!currentStatus ? "true" : "false") + "}";
    request->send(200, "application/json", jsonResponse);
  });

  // Fetch data
  server.on("/fetchData", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (SPIFFS.exists("/data.txt")) {
      request->send(SPIFFS, "/data.txt", "text/plain");
    } else {
      request->send(404, "text/plain", "Data not found.");
    }
  });

  // Get sensor data
  server.on("/getSensorData", HTTP_GET, [](AsyncWebServerRequest *request) {
    float temperature = bme.readTemperature();
    float humidity = bme.readHumidity();

    String jsonResponse = "{ \"temperature\": " + String(temperature) + ", \"humidity\": " + String(humidity) + " }";
    request->send(200, "application/json", jsonResponse);
  });

  // Start the server
  server.begin();
}

// Function to save the desired status to SPIFFS
void saveDesiredStatus(bool desiredStatus) {
  fs::File file = SPIFFS.open("/desired_status.txt", "w");
  
  if (!file) {
    Serial.println("Error opening desired_status.txt for writing");
    displayError("PLS CHECK DOCS:", "071");
    return;
  }

  file.println(desiredStatus ? "1" : "0");
  file.close();
}

// Function to read the desired status from SPIFFS
bool getDesiredStatus() {
  fs::File file = SPIFFS.open("/desired_status.txt", "r");
  
  if (!file) {
    Serial.println("Error opening desired_status.txt for reading");
    displayError("PLS CHECK DOCS:", "071");
    return true; // Default to true if there is an error
  }

  bool desiredStatus = file.parseInt() == 1;
  file.close();

  return desiredStatus;
}

// Function to read the desired temperature from SPIFFS
float desiredTempFromDb() {
  // Read the desired temperature from SPIFFS
  fs::File file = SPIFFS.open("/desired_temp.txt", "r");
  if (!file) {
    Serial.println("Error opening desired_temp.txt for reading");
    return -500;
  }
  float res = file.parseFloat();
  file.close();
  return res;
}

// Function to read the desired humidity from SPIFFS
int desiredHumFromDb() {
  // Read the desired humidity from SPIFFS
  fs::File file = SPIFFS.open("/desired_hum.txt", "r");
  if (!file) {
    Serial.println("Error opening file for reading");
    displayError("error opening file");
    return -500;
  }
  int res = file.parseInt();
  file.close();
  return res;
}

// Function to send data to the database
void sendToDatabase(float tempSensor, int humSensor) {
  fs::File file = SPIFFS.open("/data.txt", "a");
  
  if (!file) {
    Serial.println("Error opening data.txt for writing");
    displayError("091");
    return;
  }

  file.println(String(tempSensor) + "," + String(humSensor) + "   ");
  file.close();

  Serial.println("Data saved to SPIFFS");
}

// Function to control the relay
void relayControl(float tempSensor, float tempDb) {
  if (tempSensor >= tempDb) {
    digitalWrite(relayPin, OFF); // HEATER OFF
  } else {
    digitalWrite(relayPin, ON); // HEATER ON
  }
}

// Function to control the servo motor
void servoControl(int humSensor, int humDb) {
  if (humSensor > humDb) {
    myservo.write(SERVO_OPEN); // ventilation opens
  } else {
    myservo.write(SERVO_CLOSED); // ventilation closes
  }
}

// Function to read the desired temperature from SPIFFS
void setupStorage() {
  // Initialize SPIFFS (file system)
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    displayError("Error mounting SPIFFS");
    return;
  }
  // Display success message on TFT screen
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.println("SPIFFS mounted successfully!");
  
  // Print success message on serial monitor
  Serial.println("SPIFFS mounted successfully!");
}

void displayError(const String &errorMessage, const String &errorCode) {
  tft.fillScreen(BLACK);
  tft.setCursor(0, 0);
  tft.print(errorMessage);

  if (!errorCode.isEmpty()) {
    tft.setCursor(0, 20);
    tft.print("ERROR CODE: " + errorCode);
  } else {
    tft.setCursor(0, 20);
    tft.print("PLS CHECK LOG");
  }
}