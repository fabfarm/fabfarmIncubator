#ifndef WEB_SERVER_MANAGER_H
#define WEB_SERVER_MANAGER_H

// creating a header file first and then including it in the main file
// is a good practice to avoid circular dependencies

#include "config.h"

// Function prototypes

bool getIncubatorStatus();
void loadSettings();
void initializeWebServer();
void handleRootRequest(AsyncWebServerRequest *request);
void handleTemperatureSettingsUpdate(AsyncWebServerRequest *request);
void handleTemperatureSettingsRequest(AsyncWebServerRequest *request);
void handleHumiditySettingsUpdate(AsyncWebServerRequest *request);
void handleHumiditySettingsRequest(AsyncWebServerRequest *request);
void handleServoAngleUpdate(AsyncWebServerRequest *request);
void handleServoAngleRequest(AsyncWebServerRequest *request);
void handleServoIntervalUpdate(AsyncWebServerRequest *request);
void handleServoIntervalRequest(AsyncWebServerRequest *request);
void handleDebugModeToggle(AsyncWebServerRequest *request);
void handleDebugModeRequest(AsyncWebServerRequest *request);
void handleIncubatorStatusToggle(AsyncWebServerRequest *request);
void handleIncubatorStatusRequest(AsyncWebServerRequest *request);
void handleDataFetchRequest(AsyncWebServerRequest *request);
void handleCurrentSensorDataRequest(AsyncWebServerRequest *request);

#endif