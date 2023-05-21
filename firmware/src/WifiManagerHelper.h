#ifndef WIFI_MANAGER_HELPER_H
#define WIFI_MANAGER_HELPER_H

#include <Arduino.h>
#include <WifiManagerHelper.h>

void wifiManagerSetup();

void wifiManagerLoop();

void checkButton();

String getParam(String name);

void saveParamCallback();

#endif