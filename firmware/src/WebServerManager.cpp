#include "WebServerManager.h"

#include "DebugManager.h"
#include "DisplayManager.h"
#include "FileManager.h"
#include "SensorManager.h"
#include "config.h"

void loadSettings() {
    targetTemperature     = readFromFile("/set_temp.txt").toFloat();
    targetHumidity        = readFromFile("/set_hum.txt").toInt();
    trayServoTurnAngle    = readFromFile("/trayServoTurnAngle.txt").toFloat();
    trayServoTurnInterval = readFromFile("/interval.txt").toFloat();
}

bool getIncubatorStatus() {
    return readFromFile("/set_status.txt").toInt() == 1;
}

void handleServoAngleUpdate(AsyncWebServerRequest *request) {
    String trayServoTurnAngle = request->getParam("angle")->value();
    debugMessage("Received updateServoAngle request with angle: " +
                 trayServoTurnAngle);
    writeToFile("/trayServoTurnAngle.txt", trayServoTurnAngle, false);
    request->send(200, "text/plain", "OK");
}

void handleServoAngleRequest(AsyncWebServerRequest *request) {
    String json = "{\"angle\":" + String(trayServoTurnAngle) + "}";
    request->send(200, "application/json", json);
}

void handleServoIntervalUpdate(AsyncWebServerRequest *request) {
    String trayServoTurnInterval = request->getParam("interval")->value();
    debugMessage("Received updateServoInterval request with interval: " +
                 trayServoTurnInterval);
    writeToFile("/interval.txt", trayServoTurnInterval, false);
    request->send(200, "text/plain", "OK");
}

void handleServoIntervalRequest(AsyncWebServerRequest *request) {
    String json = "{\"interval\":" + String(trayServoTurnInterval) + "}";
    request->send(200, "application/json", json);
}

void handleRootRequest(AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
}

void handleTemperatureSettingsUpdate(AsyncWebServerRequest *request) {
    String temp       = request->getParam("temp")->value();
    targetTemperature = temp.toFloat();
    debugMessage("Received updateSettings request with temp: " + temp);
    writeToFile("/set_temp.txt", String(targetTemperature), false);
    request->send(200, "text/plain", "OK");
}

void handleTemperatureSettingsRequest(AsyncWebServerRequest *request) {
    String jsonResponse =
        "{ \"temperature\": " + String(targetTemperature) + " }";
    request->send(200, "application/json", jsonResponse);
}

void handleHumiditySettingsUpdate(AsyncWebServerRequest *request) {
    String hum     = request->getParam("hum")->value();
    targetHumidity = hum.toInt();
    debugMessage("Received request with hum: " + hum);
    writeToFile("/set_hum.txt", String(targetHumidity), false);
    request->send(200, "text/plain", "OK");
}

void handleHumiditySettingsRequest(AsyncWebServerRequest *request) {
    String jsonResponse = "{ \"humidity\": " + String(targetHumidity) + " }";
    request->send(200, "application/json", jsonResponse);
}

void handleIncubatorStatusToggle(AsyncWebServerRequest *request) {
    bool currentStatus = getIncubatorStatus();
    writeToFile("/set_status.txt", currentStatus ? "0" : "1");
    String jsonResponse =
        "{\"status\": " + String(currentStatus ? "false" : "true") + "}";
    debugMessage("Toggled incubator status to: relay" +
                 String(currentStatus ? "Off" : "On"));
    request->send(200, "application/json", jsonResponse);
}

void handleIncubatorStatusRequest(AsyncWebServerRequest *request) {
    bool currentStatus = getIncubatorStatus();
    String jsonResponse =
        "{\"status\": " + String(currentStatus ? "false" : "true") + "}";
    request->send(200, "application/json", jsonResponse);
}

void handleDebugModeToggle(AsyncWebServerRequest *request) {
    // TODO: For Lucio ;)
    request->send(501, "application/json", "Not implemented yet.");
}

void getDebugModeRequest(AsyncWebServerRequest *request) {
    // TODO: For Lucio ;)
    request->send(501, "application/json", "Not implemented yet.");
}

void handleDataFetchRequest(AsyncWebServerRequest *request) {
    if (SPIFFS.exists("/data.txt")) {
        request->send(SPIFFS, "/data.txt", "text/plain");
    } else {
        request->send(404, "text/plain", "Data not found.");
    }
}

void handleCurrentSensorDataRequest(AsyncWebServerRequest *request) {
    float temperature   = bme.readTemperature();
    float humidity      = bme.readHumidity();
    String jsonResponse = "{ \"temperature\": " + String(temperature) +
                          ", \"humidity\": " + String(humidity) + " }";
    request->send(200, "application/json", jsonResponse);
}

void initializeWebServer() {
    server.on("/", HTTP_GET, handleRootRequest);
    server.on("/setTemperature", HTTP_GET, handleTemperatureSettingsUpdate);
    server.on("/getTemperature", HTTP_GET, handleTemperatureSettingsRequest);
    server.on("/setHumidity", HTTP_GET, handleHumiditySettingsUpdate);
    server.on("/getHumidity", HTTP_GET, handleHumiditySettingsRequest);

    server.on("/setAngle", HTTP_GET, handleServoAngleUpdate);
    server.on("/getAngle", HTTP_GET, handleServoAngleRequest);
    server.on("/setInterval", HTTP_GET, handleServoIntervalUpdate);
    server.on("/getInterval", HTTP_GET, handleServoIntervalRequest);

    server.on("/toggleIncubator", HTTP_GET, handleIncubatorStatusToggle);
    server.on("/getIncubatorOperationStatus", HTTP_GET,
              handleIncubatorStatusRequest);

    server.on("/toggleDebugMode", HTTP_GET, handleDebugModeToggle);
    server.on("/getDebugMode", HTTP_GET, handleDebugModeRequest);

    server.on("/fetchData", HTTP_GET, handleDataFetchRequest);
    server.on("/getCurrentSensorData", HTTP_GET,
              handleCurrentSensorDataRequest);
    server.begin();
}