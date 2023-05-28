#ifndef RUN_INCUBATOR_h
#define RUN_INCUBATOR_h

#include "Config.h"

#include "DebugManager.h"
#include "DisplayManager.h"
#include "FileManager.h"
#include "SensorManager.h"
#include "ServoManager.h"
#include "WebServerManager.h"

void runIncubator();

void controlHumidityVentServo(int targetHumidity);
void controlHeatElementMosfet(float targetTemperature);
bool hasIntervalPassed(unsigned long intervalMillis, unsigned long &lastCheck);
void pauseSystem();
void saveData();
bool getIncubatorStatus();

#endif