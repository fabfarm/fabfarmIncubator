// Fabfarm Egg Incubator

#include "DebugManager.h"
#include "DisplayManager.h"
#include "FileManager.h"
#include "SensorManager.h"
#include "ServoManager.h"
#include "WebServerManager.h"
#include "WifiManagerHelper.h"
#include "config.h"
#include "pidManager.h"
#include "runIncubator.h"

void setup() {
    wifiManagerSetup();
    Serial.begin(115200);
    pinMode(mosfetPin, OUTPUT);
    setupPidControllers();
    // initializeTFTDisplay();
    initializeStorage();
    connectServos();
    loadSettings();
    initializeSensors();
    initializeWebServer();
    debugMessage("Setup complete");
}

void loop() {
    wifiManagerLoop();
    runIncubator();
}