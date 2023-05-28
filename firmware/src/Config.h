#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <WiFiManager.h>
// The wifiManager Library has to be placed right after the Arduino.h library or
// else it will not compile
#include <Adafruit_BME280.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <PID_v1.h>
#include <SPIFFS.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <Wire.h>
#include <math.h>
#include "NotoSansBold15.h"

extern TFT_eSPI tft;


#define BME_SDA BMESdaPin
#define BME_SCL BMESclPin
#define ON LOW
#define OFF HIGH
#define hysteresis 0.2  // degrees Celsius
#define TRIGGER_PIN 0

extern bool      debugMode;
extern float       currentHumidity;
extern float     currentPressure;
extern float     currentTemperature;
extern float     targetTemperature;
extern int       targetHumidity;
extern float     trayServoTurnInterval;
extern float     trayServoTurnAngle;


// timeIntervalToSaveData
extern unsigned long timeIntervalToSaveData;

extern const int mosfetPin;
extern const int humidityVentServoPin;
extern const int trayServoPin;

extern const int BMESdaPin;
extern const int BMESclPin;

extern const int servoOpenPosition;
extern const int servoClosedPosition;

extern int timeout;

extern Adafruit_BME280 bme;
extern TwoWire         I2CBME;

extern AsyncWebServer server;

extern Servo ventServo;
extern Servo trayServo;

extern PID    tempPID;
extern PID    humPID;
extern double tempSetpoint, tempInput, tempOutput;
extern double humSetpoint, humInput, humOutput;
extern double tempKp, tempKi, tempKd;
extern double humKp, humKi, humKd;

extern bool        wm_nonblocking;  // change to true to use non blocking
extern WiFiManager wm;              // global wm instance
extern WiFiManagerParameter
    custom_field;  // global param ( for non blocking w params )

#endif
