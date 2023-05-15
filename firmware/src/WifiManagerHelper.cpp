#include <Arduino.h>
#include <WiFiManager.h>

#include "config.h"
#include "WebServerManager.h"
#include "WifiManagerHelper.h"
#include "DebugManager.h"

void wifiManagerSetup() {
  WiFiManager wm;
  // reset settings - wipe stored credentials for testing
  // these are stored by the esp library
  // wm.resetSettings();
  if (!wm.autoConnect("AutoConnectAP")) {
    debugMessage("Failed to connect");
    ESP.restart();
  }
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  debugMessage("Connected to WiFi");
}

void wifiManagerTrigerPinSetup() {
  if ( digitalRead(TRIGGER_PIN) == LOW) {
    WiFiManager wm;   
    wm.setConfigPortalTimeout(timeout);
    if (!wm.startConfigPortal("OnDemandAP")) {
      debugMessage("Failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.restart();
      delay(5000);
    }
    //if you get here you have connected to the WiFi
    debugMessage("Connected to WiFi");
  }
}
