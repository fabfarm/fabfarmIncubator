#ifndef WEB_SERVER_MANAGER_H
#define WEB_SERVER_MANAGER_H

// creating a header file first and then including it in the main file
// is a good practice to avoid circular dependencies

#include "config.h"

// Function prototypes

bool    getIncubatorStatus();
void    loadSettings();
void    initializeWebServer();
void    handleRootRequest(AsyncWebServerRequest *request);
void    handleTemperatureHumiditySettingsUpdate(AsyncWebServerRequest *request);
void    handleIncubatorStatusToggle(AsyncWebServerRequest *request);
void    handleDataFetchRequest(AsyncWebServerRequest *request);
void    handleSensorDataRequest(AsyncWebServerRequest *request);
void    handleCurrentSettingsRequest(AsyncWebServerRequest *request);
void    handleServoSettingsUpdate(AsyncWebServerRequest *request);
void    handleCurrentServoSettingsRequest(AsyncWebServerRequest *request);
void    handleCurrentPidSettingsRequest(AsyncWebServerRequest *request);
void    handlePidSettingsUpdate(AsyncWebServerRequest *request);

#endif