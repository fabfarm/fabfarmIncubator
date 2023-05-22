#include "WebServerManager.h"
struct DataPoint {
    float x;
    float y;
};

std::vector<DataPoint> extractDataFromCSV(const String &filename) {
    std::vector<DataPoint> dataPoints;

    fs::File file = readFile(filename.c_str());
    if (!file) {
        Serial.println("Failed to open file: " + filename);
        return dataPoints;
    }

    String line;
    while (file.available()) {
        line = file.readStringUntil('\n');
        line.trim();

        int commaIndex = line.indexOf(',');
        if (commaIndex == -1) {
            Serial.println("Invalid line format: " + line);
            continue;
        }

        String valueX = line.substring(0, commaIndex);
        String valueY = line.substring(commaIndex + 1);

        float x = valueX.toFloat();
        float y = valueY.toFloat();

        DataPoint dataPoint;
        dataPoint.x = x;
        dataPoint.y = y;

        dataPoints.push_back(dataPoint);
    }

    file.close();

    return dataPoints;
}
String restructureDataToJson(const std::vector<DataPoint> &temperatureData,
                             const std::vector<DataPoint> &humidityData) {
    debugMessage("Restructuring data to JSON");
    DynamicJsonDocument doc(2048);

    JsonArray  rootArr     = doc.createNestedArray();
    JsonObject temperature = rootArr.createNestedObject();
    JsonObject humidity    = rootArr.createNestedObject();
    temperature["name"]    = "Daily Temperature";
    humidity["name"]       = "Daily Humidity";

    JsonArray tempDataArr = temperature.createNestedArray("data");
    JsonArray humDataArr  = humidity.createNestedArray("data");

    for (const auto &dataPoint : humidityData) {
        JsonObject humData = humDataArr.createNestedObject();
        humData["x"]       = dataPoint.x;
        humData["y"]       = dataPoint.y;
    }

    for (const auto &dataPoint : temperatureData) {
        JsonObject tempData = tempDataArr.createNestedObject();
        tempData["x"]       = dataPoint.x;
        tempData["y"]       = dataPoint.y;
    }

    String output;
    // TODO: No output here, why can't the csv files be opened?
    serializeJson(doc, output);
    return output;
}

void loadSettings() {
    targetTemperature     = readFromFile("/set_temp.txt").toFloat();
    targetHumidity        = readFromFile("/set_hum.txt").toInt();
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
    writeToFile("/trayServoInterval.txt", trayServoTurnInterval, false);
    request->send(200, "text/plain", "OK");
}

void handleServoIntervalRequest(AsyncWebServerRequest *request) {
    String json = "{\"interval\":" + String(trayServoTurnInterval) + "}";
    request->send(200, "application/json", json);
}

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
    writeToFile("/set_status.txt", currentStatus ? "0" : "1", false);
    debugMessage("Incubator status file contents" +
                 readFromFile("/set_status.txt"));

    String jsonResponse =
        "{\"status\": " + String(currentStatus ? "false" : "true") + "}";
    debugMessage("Toggled incubator status to:" +
                 String(currentStatus ? "Off" : "On"));
    request->send(200, "application/json", jsonResponse);
}

void handleIncubatorStatusRequest(AsyncWebServerRequest *request) {
    bool   currentStatus = getIncubatorStatus();
    String jsonResponse =
        "{\"status\": " + String(currentStatus ? "true" : "false") + "}";
    request->send(200, "application/json", jsonResponse);
}

void handleDebugModeToggle(AsyncWebServerRequest *request) {
    // TODO: For Lucio ;)
    request->send(501, "application/json", "Not implemented yet.");
}

void handleDebugModeRequest(AsyncWebServerRequest *request) {
    // TODO: For Lucio ;)
    request->send(501, "application/json", "Not implemented yet.");
}

void handleDataFetchRequest(AsyncWebServerRequest *request) {
    // File paths for temperature and humidity CSV files
    String temperatureFileName = "/data_temp.csv";
    String humidityFileName    = "/data_hum.csv";
    if (SPIFFS.exists(temperatureFileName) && SPIFFS.exists(humidityFileName)) {
        request->send(
            200, "application/json",
            restructureDataToJson(extractDataFromCSV(temperatureFileName),
                                  extractDataFromCSV(humidityFileName)));
    } else {
        request->send(404, "text/plain", "Data not found.");
    }
}

void handleGetCurrentTemperature(AsyncWebServerRequest *request) {
    float  temperature  = bme.readTemperature();
    String jsonResponse = "{ \"temperature\": " + String(temperature) + " }";
    request->send(200, "application/json", jsonResponse);
}

void handleGetCurrentHumidity(AsyncWebServerRequest *request) {
    float  humidity     = bme.readHumidity();
    String jsonResponse = "{ \"humidity\": " + String(humidity) + " }";
    request->send(200, "application/json", jsonResponse);
}

void handleResetDataRequest(AsyncWebServerRequest *request) {
    if (SPIFFS.exists("/data_temp.csv") && SPIFFS.exists("/data_hum.csv")) {
        writeToFile("/data_temp.csv", "", false);
        writeToFile("/data_hum.csv", "", false);
        debugMessage("DATA RESET");
        request->send(200, "text/plain", "Data was reset.");
    } else {
        request->send(404, "text/plain", "Data not found.");
    }
}

void initializeWebServer() {
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader(
        "Access-Control-Allow-Methods",
        "GET, POST, PATCH, PUT, DELETE, OPTIONS");
    server.serveStatic("/assets/", SPIFFS, "/assets/");
    server.on("/", HTTP_GET, handleRootRequest);
    server.on("/setTargetTemperature", HTTP_POST,
              handleTemperatureSettingsUpdate);
    server.on("/getTargetTemperature", HTTP_GET,
              handleTemperatureSettingsRequest);
    server.on("/getCurrentTemperature", HTTP_GET, handleGetCurrentTemperature);
    server.on("/setTargetHumidity", HTTP_POST, handleHumiditySettingsUpdate);
    server.on("/getTargetHumidity", HTTP_GET, handleHumiditySettingsRequest);
    server.on("/getCurrentHumidity", HTTP_GET, handleGetCurrentHumidity);
    server.on("/setAngle", HTTP_POST, handleServoAngleUpdate);
    server.on("/getAngle", HTTP_GET, handleServoAngleRequest);
    server.on("/setInterval", HTTP_POST, handleServoIntervalUpdate);
    server.on("/getInterval", HTTP_GET, handleServoIntervalRequest);

    server.on("/toggleIncubator", HTTP_POST, handleIncubatorStatusToggle);
    server.on("/toggleDebugMode", HTTP_POST, handleDebugModeToggle);
    server.on("/getDebugMode", HTTP_GET, handleDebugModeRequest);

    server.on("/fetchData", HTTP_GET, handleDataFetchRequest);
    server.on("/resetData", HTTP_POST, handleResetDataRequest);
    server.on("/getCurrentPidSettings", HTTP_GET,
              handleCurrentPidSettingsRequest);
    server.on("/updatePidSettings", HTTP_POST, handlePidSettingsUpdate);

    // for ./assets/barebone.html
    server.on("/getIncubatorStatus", HTTP_GET, handleIncubatorStatusRequest);
    server.on("/updateServoSettings", HTTP_GET, handleServoSettingsUpdate);
    server.on("/getCurrentSettings", HTTP_GET, handleCurrentSettingsRequest);
    server.on("/getCurrentServoSettings", HTTP_GET,
              handleCurrentServoSettingsRequest);
    server.on("/getSensorData", HTTP_GET, handleSensorDataRequest);
    server.on("/updateSettings", HTTP_GET,
              handleTemperatureHumiditySettingsUpdate);
    server.begin();
}

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
void handleCurrentServoSettingsRequest(AsyncWebServerRequest *request) {
    String json = "{\"angle\":" + String(trayServoTurnAngle) +
                  ",\"interval\":" + String(trayServoTurnInterval) + "}";
    request->send(200, "application/json", json);
}
void handleCurrentSettingsRequest(AsyncWebServerRequest *request) {
    String json = "{\"temp\":" + String(targetTemperature) +
                  ",\"hum\":" + String(targetHumidity) + "}";
    request->send(200, "application/json", json);
}
void handleTemperatureHumiditySettingsUpdate(AsyncWebServerRequest *request) {
    String temp       = request->getParam("temp")->value();
    String hum        = request->getParam("hum")->value();
    targetTemperature = temp.toFloat();
    targetHumidity    = hum.toInt();
    debugMessage("Received updateSettings request with temp: " + temp +
                 " and hum: " + hum);
    writeToFile("/set_temp.txt", String(targetTemperature), false);
    writeToFile("/set_hum.txt", String(targetHumidity), false);
    request->send(200, "text/plain", "OK");
}
void handleSensorDataRequest(AsyncWebServerRequest *request) {
    float  temperature  = bme.readTemperature();
    float  humidity     = bme.readHumidity();
    String jsonResponse = "{ \"temperature\": " + String(temperature) +
                          ", \"humidity\": " + String(humidity) + " }";
    request->send(200, "application/json", jsonResponse);
}
void handleCurrentPidSettingsRequest(AsyncWebServerRequest *request) {
    String json =
        "{\"tempKp\":" + String(tempKp) + ",\"tempKi\":" + String(tempKi) +
        ",\"tempKd\":" + String(tempKd) + ",\"humKp\":" + String(humKp) +
        ",\"humKi\":" + String(humKi) + ",\"humKd\":" + String(humKd) + "}";
    request->send(200, "application/json", json);
}
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