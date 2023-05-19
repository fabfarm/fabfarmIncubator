// Fabfarm Egg Incubator
// Written by: Lucio

#include "DebugManager.h"
#include "DisplayManager.h"
#include "FileManager.h"
#include "SensorManager.h"
#include "ServoManager.h"
#include "WebServerManager.h"
#include "WifiManagerHelper.h"
#include "pidManager.h"



void setup() {
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  wifiManagerSetup();
  AsyncElegantOTA.begin(&server);
  Serial.begin(115200);
  initializeTFTDisplay();
  initializeStorage();
  loadSettings();
  initializeWebServer();
  connectServos();
  initializeSensors();
  setupPidControllers();
  debugMessage("Setup complete");
  pinMode(mosfetPin, OUTPUT);
}

void loop() {
    wifiManagerTrigerPinSetup();
    runIncubator();
}