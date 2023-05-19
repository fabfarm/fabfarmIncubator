#include "config.h"
#include "WebServerManager.h"
#include "FileManager.h"
#include "DisplayManager.h"
#include "SensorManager.h"
#include "DebugManager.h"

void  handleRootRequest(AsyncWebServerRequest *request) {
  loadSettings();
  request->send(SPIFFS, "/index.html", "text/html");
}

void  handleNotFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void handleCurrentPidSettingsRequest(AsyncWebServerRequest *request) {
  String json = "{\"tempKp\":" + String(tempKp) + ",\"tempKi\":" + String(tempKi) + ",\"tempKd\":" + String(tempKd) + ",\"humKp\":" + String(humKp) + ",\"humKi\":" + String(humKi) + ",\"humKd\":" + String(humKd) + "}";
  request->send(200, "application/json", json);
}
void handlePidSettingsUpdate(AsyncWebServerRequest *request) {
    String tempKp     = request->getParam("tempKp")->value();
    String tempKi     = request->getParam("tempKi")->value();
    String tempKd     = request->getParam("tempKd")->value();
    String humKp      = request->getParam("humKp")->value();
    String humKi      = request->getParam("humKi")->value();
    String humKd      = request->getParam("humKd")->value();
    debugMessage("Received updatePidSettings request with tempKp: " + tempKp + " tempKi: " + tempKi + " tempKd: " + tempKd + " humKp: " + humKp + " humKi: " + humKi + " humKd: " + humKd);
    writeToFile("/tempKp.txt", String(tempKp), false);
    writeToFile("/tempKi.txt", String(tempKi), false);
    writeToFile("/tempKd.txt", String(tempKd), false);
    writeToFile("/humKp.txt", String(humKp), false);
    writeToFile("/humKi.txt", String(humKi), false);
    writeToFile("/humKd.txt", String(humKd), false);
    request->send(200, "text/plain", "OK");
}

void  loadSettings() {
  targetTemperature = readFromFile("/set_temp.txt").toFloat();
  targetHumidity = readFromFile("/set_hum.txt").toInt();
  trayServoTurnAngle = readFromFile("/trayServoTurnAngle.txt").toFloat();
  trayServoTurnInterval = readFromFile("/interval.txt").toFloat();
  tempKp = readFromFile("/tempKp.txt").toFloat();
  tempKi = readFromFile("/tempKi.txt").toFloat();
  tempKd = readFromFile("/tempKd.txt").toFloat();
  humKp = readFromFile("/humKp.txt").toFloat();
  humKi = readFromFile("/humKi.txt").toFloat();
  humKd = readFromFile("/humKd.txt").toFloat();
  debugMessage("Loaded settings from files");
}
void handleCurrentSettingsRequest(AsyncWebServerRequest *request) {
  String json = "{\"temp\":" + String(targetTemperature) + ",\"hum\":" + String(targetHumidity) + "}";
  request->send(200, "application/json", json);
}
void  handleTemperatureHumiditySettingsUpdate(AsyncWebServerRequest *request) {
  String temp = request->getParam("temp")->value();
  String hum = request->getParam("hum")->value();
  targetTemperature = temp.toFloat();
  targetHumidity = hum.toInt();
  debugMessage("Received updateSettings request with temp: " + temp + " and hum: " + hum);
  writeToFile("/set_temp.txt", String(targetTemperature), false);
  writeToFile("/set_hum.txt", String(targetHumidity), false);
  request->send(200, "text/plain", "OK");
}

void  handleCurrentServoSettingsRequest(AsyncWebServerRequest *request) {
  String json = "{\"angle\":" + String(trayServoTurnAngle) + ",\"interval\":" + String(trayServoTurnInterval) + "}";	
  request->send(200, "application/json", json);
}

void  handleServoSettingsUpdate(AsyncWebServerRequest *request) {
  String trayServoTurnAngle      = request->getParam("angle")->value();
  String trayServoTurnInterval   = request->getParam("interval")->value();
  debugMessage("Received updateServoSettings request with angle: " + trayServoTurnAngle + " and interval: " + trayServoTurnInterval);
  writeToFile("/trayServoTurnAngle.txt", trayServoTurnAngle, false);
  writeToFile("/interval.txt", trayServoTurnInterval, false);
  request->send(200, "text/plain", "OK");
}

bool  getIncubatorStatus() {
  return readFromFile("/set_status.txt").toInt() == 1;
}

void  handleIncubatorStatusToggle(AsyncWebServerRequest *request) {
  bool currentStatus = getIncubatorStatus();
  writeToFile("/set_status.txt", currentStatus ? "0" : "1");
  String jsonResponse = "{\"status\": " + String(currentStatus ? "false" : "true") + "}";
  debugMessage("Toggled incubator status to: relay" + String(currentStatus ? "Off" : "On"));
  request->send(200, "application/json", jsonResponse);
}

void  handleDataFetchRequest(AsyncWebServerRequest *request) {
  if (SPIFFS.exists("/data.txt")) {
    request->send(SPIFFS, "/data.txt", "text/plain");
  } else {
    request->send(404, "text/plain", "Data not found.");
  }
}

void  handleSensorDataRequest(AsyncWebServerRequest *request) {
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  String jsonResponse = "{ \"temperature\": " + String(temperature) + ", \"humidity\": " + String(humidity) + " }";
  request->send(200, "application/json", jsonResponse);
}

void  initializeWebServer() {
  server.on("/", HTTP_GET, handleRootRequest);
  server.on("/getSensorData", HTTP_GET, handleSensorDataRequest);
  server.on("/getCurrentSettings", HTTP_GET, handleCurrentSettingsRequest);
  server.on("/updateSettings", HTTP_GET, handleTemperatureHumiditySettingsUpdate);
  server.on("/getCurrentServoSettings", HTTP_GET, handleCurrentServoSettingsRequest);
  server.on("/updateServoSettings", HTTP_GET, handleServoSettingsUpdate);
  server.on("/getCurrentPidSettings", HTTP_GET, handleCurrentPidSettingsRequest);
  server.on("/updatePidSettings", HTTP_GET, handlePidSettingsUpdate);
  server.on("/toggleIncubator", HTTP_GET, handleIncubatorStatusToggle);
  server.on("/fetchData", HTTP_GET, handleDataFetchRequest);
  server.begin();
}