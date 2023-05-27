// Fabfarm Egg Incubator

#include "DebugManager.h"
#include "DisplayManager.h"
#include "FileManager.h"
#include "SensorManager.h"
#include "ServoManager.h"
#include "WebServerManager.h"
#include "WifiManagerHelper.h"
#include "Config.h"
#include "PidManager.h"
#include "RunIncubator.h"

void setup() {
    initializeTFTDisplay();
    wifiManagerSetup();
    Serial.begin(115200);
    pinMode(mosfetPin, OUTPUT);
    setupPidControllers();
    initializeStorage();
    connectServos();
    loadSettings();
    initializeSensors();
    initializeWebServer();
    debugMessage("Setup complete");
}

void loop() {
    // tft.fillScreen(TFT_WHITE);
    wifiManagerLoop();
    runIncubator();
}