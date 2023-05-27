#ifndef WEB_SERVER_MANAGER_H
#define WEB_SERVER_MANAGER_H

#include "Config.h"

#include "DebugManager.h"
#include "DisplayManager.h"
#include "FileManager.h"

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
void handleCurrentPidSettingsRequest(AsyncWebServerRequest *request);
void handlePidSettingsUpdate(AsyncWebServerRequest *request);
void handleTemperatureHumiditySettingsUpdate(AsyncWebServerRequest *request);
void handleCurrentServoSettingsRequest(AsyncWebServerRequest *request);
void handleSensorDataRequest(AsyncWebServerRequest *request);
void handleCurrentSettingsRequest(AsyncWebServerRequest *request);
void handleServoSettingsUpdate(AsyncWebServerRequest *request);

#endif