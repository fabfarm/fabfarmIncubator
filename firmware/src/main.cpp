// Fabfarm Egg Incubator

#include "WebServerManager.h"
#include "config.h"
#include "FileManager.h"
#include "WifiManagerHelper.h"
#include "DisplayManager.h"
#include "SensorManager.h"
#include "ServoManager.h"
#include "DebugManager.h"
#include "runIncubator.h"
#include "pidManager.h"



void setup() {
  wifiManagerSetup();
  Serial.begin(115200);
  pinMode(mosfetPin, OUTPUT);
  setupPidControllers();
  initializeTFTDisplay();
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