#include "config.h"

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

Adafruit_BME280   bme;
TwoWire           I2CBME        = TwoWire(0);

int timeout = 120;

AsyncWebServer  server(80);

Servo           ventServo;
Servo           trayServo;
TFT_eSPI        tft              = TFT_eSPI();
int16_t         displayHeight    = 128;
int16_t         displayWidth     = 160;

double    tempSetpoint, tempInput,  tempOutput;
double    humSetpoint,  humInput,   humOutput;
double    tempKp, tempKi, tempKd;
double    humKp, humKi, humKd;

PID tempPID(&tempInput, &tempOutput,  &tempSetpoint,  tempKp, tempKi, tempKd, DIRECT);
PID humPID(&humInput,   &humOutput,   &humSetpoint,   humKp,  humKi,  humKd,  DIRECT);
