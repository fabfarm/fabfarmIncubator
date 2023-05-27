#include "WebServerManager.h"

void loadSettings() {
    targetTemperature     = readFromFile("/setTemp.txt").toFloat();
    targetHumidity        = readFromFile("/setHum.txt").toInt();
    trayServoTurnAngle    = readFromFile("/trayServoTurnAngle.txt").toFloat();
    trayServoTurnInterval = readFromFile("/trayServoInterval.txt").toFloat();
    tempKp                = readFromFile("/tempKp.txt").toFloat();
    tempKi                = readFromFile("/tempKi.txt").toFloat();
    tempKd                = readFromFile("/tempKd.txt").toFloat();
    humKp                 = readFromFile("/humKp.txt").toFloat();
    humKi                 = readFromFile("/humKi.txt").toFloat();
    humKd                 = readFromFile("/humKd.txt").toFloat();
    debugMessage("Loaded settings from files");
}

void initializeWebServer() {
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods",
                                         "GET, POST, PUT, DELETE, OPTIONS");
    // for ./assets/barebone.html
    // 1
    server.on("/", HTTP_GET, handleRootRequest);
    // ok
    // 2
    server.on("/toggleIncubator", HTTP_GET, handleIncubatorStatusToggle);
    // ok
    // 3
    server.on("/fetchData", HTTP_GET, handleDataFetchRequest);
    // ok
    // 4
    server.on("/getCurrentPidSettings", HTTP_GET,
              handleCurrentPidSettingsRequest);
    // ok
    // 5
    server.on("/updatePidSettings", HTTP_GET, handlePidSettingsUpdate);
    // ok
    // 6
    server.on("/getIncubatorStatus", HTTP_GET, handleIncubatorStatusRequest);
    // ok
    // 7
    server.on("/updateServoSettings", HTTP_GET, handleServoSettingsUpdate);
    // ok
    // 8
    server.on("/getCurrentSettings", HTTP_GET, handleCurrentSettingsRequest);
    // ok
    // 9
    server.on("/getCurrentServoSettings", HTTP_GET,
              handleCurrentServoSettingsRequest);
    // ok
    // 10
    server.on("/getSensorData", HTTP_GET, handleSensorDataRequest);
    // ok
    // 11
    server.on("/updateSettings", HTTP_GET,
              handleTemperatureHumiditySettingsUpdate);
    // ok
    // 12
    server.begin();
}
// 1
void handleRootRequest(AsyncWebServerRequest *request) {
    loadSettings();
    // Set the appropriate Content-Type for each file
    if (request->url().endsWith(".gz")) {
        AsyncWebServerResponse *response = request->beginResponse(
            SPIFFS, request->url(), "application/octet-stream");
        response->addHeader("Content-Encoding", "gzip");
        request->send(response);
    } else {
        request->send(SPIFFS, "/index.html", "text/html");
    }
}
// 2
void handleIncubatorStatusToggle(AsyncWebServerRequest *request) {
    bool currentStatus = getIncubatorStatus();
    writeToFile("/setStatus.txt", currentStatus ? "0" : "1", false);
    String jsonResponse =
        "{\"status\": " + String(currentStatus ? "false" : "true") + "}";
    debugMessage("Toggled incubator status to:" +
                 String(currentStatus ? "Off" : "On"));
    request->send(200, "application/json", jsonResponse);
}
// 3
void handleDataFetchRequest(AsyncWebServerRequest *request) {
    if (SPIFFS.exists("/data.txt")) {
        request->send(SPIFFS, "/data.txt", "text/plain");
    } else {
        request->send(404, "text/plain", "Data not found.");
    }
}
// 4
void handleCurrentPidSettingsRequest(AsyncWebServerRequest *request) {
    String json =
        "{\"tempKp\":" + String(tempKp) + ",\"tempKi\":" + String(tempKi) +
        ",\"tempKd\":" + String(tempKd) + ",\"humKp\":" + String(humKp) +
        ",\"humKi\":" + String(humKi) + ",\"humKd\":" + String(humKd) + "}";
    request->send(200, "application/json", json);
}
// 5
void handlePidSettingsUpdate(AsyncWebServerRequest *request) {
    String tempKp = request->getParam("tempKp")->value();
    String tempKi = request->getParam("tempKi")->value();
    String tempKd = request->getParam("tempKd")->value();
    String humKp  = request->getParam("humKp")->value();
    String humKi  = request->getParam("humKi")->value();
    String humKd  = request->getParam("humKd")->value();
    debugMessage("Received updatePidSettings request with tempKp: " + tempKp +
                 " tempKi: " + tempKi + " tempKd: " + tempKd +
                 " humKp: " + humKp + " humKi: " + humKi + " humKd: " + humKd);
    writeToFile("/tempKp.txt", String(tempKp), false);
    writeToFile("/tempKi.txt", String(tempKi), false);
    writeToFile("/tempKd.txt", String(tempKd), false);
    writeToFile("/humKp.txt", String(humKp), false);
    writeToFile("/humKi.txt", String(humKi), false);
    writeToFile("/humKd.txt", String(humKd), false);
    request->send(200, "text/plain", "OK");
}
// 6
void handleIncubatorStatusRequest(AsyncWebServerRequest *request) {
    bool   currentStatus = getIncubatorStatus();
    String jsonResponse =
        "{\"status\": " + String(currentStatus ? "false" : "true") + "}";
    request->send(200, "application/json", jsonResponse);
}
// 7
void handleServoSettingsUpdate(AsyncWebServerRequest *request) {
    String trayServoTurnAngle    = request->getParam("angle")->value();
    String trayServoTurnInterval = request->getParam("interval")->value();
    debugMessage("Received updateServoSettings request with angle: " +
                 trayServoTurnAngle +
                 " and interval: " + trayServoTurnInterval);
    writeToFile("/trayServoTurnAngle.txt", trayServoTurnAngle, false);
    writeToFile("/trayServoInterval.txt", trayServoTurnInterval, false);
    request->send(200, "text/plain", "OK");
}
// 8
void handleCurrentSettingsRequest(AsyncWebServerRequest *request) {
    String json = "{\"temp\":" + String(targetTemperature) +
                  ",\"hum\":" + String(targetHumidity) + "}";
    request->send(200, "application/json", json);
}
// 9
void handleCurrentServoSettingsRequest(AsyncWebServerRequest *request) {
    String json = "{\"angle\":" + String(trayServoTurnAngle) +
                  ",\"interval\":" + String(trayServoTurnInterval) + "}";
    request->send(200, "application/json", json);
}
// 10
void handleSensorDataRequest(AsyncWebServerRequest *request) {
    float  temperature  = bme.readTemperature();
    float  humidity     = bme.readHumidity();
    String jsonResponse = "{ \"temperature\": " + String(temperature) +
                          ", \"humidity\": " + String(humidity) + " }";
    request->send(200, "application/json", jsonResponse);
}
// 11
void handleTemperatureHumiditySettingsUpdate(AsyncWebServerRequest *request) {
    String temp       = request->getParam("temp")->value();
    String hum        = request->getParam("hum")->value();
    targetTemperature = temp.toFloat();
    targetHumidity    = hum.toInt();
    debugMessage("Received updateSettings request with temp: " + temp +
                 " and hum: " + hum);
    writeToFile("/setTemp.txt", String(targetTemperature), false);
    writeToFile("/setHum.txt", String(targetHumidity), false);
    request->send(200, "text/plain", "OK");
}