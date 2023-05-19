#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include <WiFiManager.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <math.h>
#include <ESP32Servo.h>
#include <PID_v1.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <ESPAsyncWebServer.h>
#include "NotoSansBold15.h"
#include <AsyncElegantOTA.h> // For over-the-air updates

extern bool      debugMode;
extern int       currentHumidity;
extern float     currentPressure;
extern float     currentTemperature;
extern float     targetTemperature;
extern int       targetHumidity;
extern float     trayServoTurnInterval;
extern float     trayServoTurnAngle;

// Temperature in Celsius
extern float     minTemperature;    
extern float     maxTemperature;
extern float     minHumidity;
extern float     maxHumidity;

extern const int mosfetPin;
extern const int humidityVentServoPin;
extern const int trayServoPin;

extern const int BMESdaPin;   //Board SDA to sensor SDI
extern const int BMESclPin;   //Board SCL to sensor SCK

#define BME_SDA BMESdaPin
#define BME_SCL BMESclPin

extern const int servoOpenPosition;
extern const int servoClosedPosition;

extern Adafruit_BME280   bme;
extern TwoWire           I2CBME;

#define       ON                  LOW
#define       OFF                 HIGH 

#define       BLACK            0x0000
#define       WHITE            0xFFFF
#define       RED              0xF800

#define       hysteresis       0.2 // degrees Celsius

// select which pin will trigger the configuration portal when set to LOW
#define TRIGGER_PIN 0
extern int timeout;

extern AsyncWebServer  server;
extern Servo           ventServo;
extern Servo           trayServo;
extern TFT_eSPI        tft;
extern int16_t         displayHeight;
extern int16_t         displayWidth;

extern double    tempSetpoint, tempInput,  tempOutput;
extern double    humSetpoint,  humInput,   humOutput;
extern double    tempKp, tempKi, tempKd;
extern double    humKp, humKi, humKd;

extern PID tempPID;
extern PID humPID;


#endif
