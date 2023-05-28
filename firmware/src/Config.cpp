#include "Config.h"

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

bool  debugMode             = true;
float currentHumidity       = 0;
float currentPressure       = 0.0;
float currentTemperature    = 0.0;
float targetTemperature     = 0.0;
int   targetHumidity        = 0;
float trayServoTurnInterval = 0;
float trayServoTurnAngle    = 0;

// timeIntervalToSaveData
unsigned long timeIntervalToSaveData = 20000;

const int mosfetPin            = 16;
const int humidityVentServoPin = 37;
const int trayServoPin         = 38;

const int BMESdaPin = 33;  // Board SDA to sensor SDI
const int BMESclPin = 34;  // Board SCL to sensor SCK

const int servoOpenPosition   = 0;
const int servoClosedPosition = 200;

Adafruit_BME280 bme;
TwoWire         I2CBME = TwoWire(0);

bool        wm_nonblocking = true;  // change to true to use non blocking
WiFiManager wm;                     // global wm instance
WiFiManagerParameter
    custom_field;  // global param ( for non blocking w params )

int timeout = 120;

AsyncWebServer server(80);

Servo    ventServo;
Servo    trayServo;

double tempSetpoint = 0, tempInput = 0, tempOutput = 0;
double humSetpoint = 0, humInput = 0, humOutput = 0;
double tempKp = 2, tempKi = 5, tempKd = 1;
double humKp = 2, humKi = 5, humKd = 1;

PID tempPID(&tempInput, &tempOutput, &tempSetpoint, tempKp, tempKi, tempKd,
            DIRECT);
PID humPID(&humInput, &humOutput, &humSetpoint, humKp, humKi, humKd, DIRECT);
