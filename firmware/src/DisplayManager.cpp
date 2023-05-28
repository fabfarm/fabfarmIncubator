#include "DisplayManager.h"

void initializeTFTDisplay() {
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_WHITE);
    tft.drawString("INITIALISING TFT...", 0, 20, 3);
    tft.fillScreen(TFT_WHITE);
}

void updateTFTDisplay() {
    currentTemperature = bme.readTemperature();
    currentHumidity    = bme.readHumidity();
    currentPressure    = bme.readPressure() / 100.0F;

    tft.drawString("T: " + String(currentTemperature, 2) + " °C", 5, 5, 2);
    tft.drawString("H: " + String(currentHumidity, 0) + " %", 5, 20, 2);
    tft.drawString("P: " + String(currentPressure, 0) + " hPa", 5, 35, 2);

    debugMessage("Temperature: " + String(currentTemperature, 2) + " °C");
    debugMessage("Humidity: " + String(currentHumidity, 2) + " %");
    debugMessage("Pressure: " + String(currentPressure, 2) + " hPa");

    tft.drawLine(0, 48, 128, 48, TFT_BLACK);

    tft.drawString("Set T: " + String(targetTemperature, 2) + " °C", 5, 50, 2);
    tft.drawString("Set H: " + String(targetHumidity, 0) + " %", 5, 65, 2);

    debugMessage("Target Temperature: " + String(targetTemperature, 2) + " °C");
    debugMessage("Target Humidity: " + String(targetHumidity, 0) + " %");
}
