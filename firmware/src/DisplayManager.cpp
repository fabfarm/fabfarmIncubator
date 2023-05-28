#include "DisplayManager.h"

void initializeTFTDisplay() {
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_WHITE);
    tft.setTextSize(2);
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
    tft.loadFont(notoFont);
    // in tft.setCursor the first parameter is the x coordinate and the second
    // is the y coordinate
    tft.setCursor(0, 20);
    tft.print("INITIALISING TFT...");
    tft.fillScreen(TFT_WHITE);
}

void updateTFTDisplay() {
    currentTemperature = bme.readTemperature();
    currentHumidity    = bme.readHumidity();
    currentPressure    = bme.readPressure() / 100.0F;
    tft.drawString("T: " + String(currentTemperature, 2) + " °C", 5, 5);
    tft.drawString("H: " + String(currentHumidity, 0) + " %", 5, 20);
    tft.drawString("P: " + String(currentPressure, 0) + " hPa", 5, 35);
    // print sensor readings to serial monitor with debugMessage()
    debugMessage("Temperature: " + String(currentTemperature, 2) + " °C");
    debugMessage("Humidity: " + String(currentHumidity, 2) + " %");
    debugMessage("Pressure: " + String(currentPressure, 2) + " hPa");

    tft.drawLine(0, 48, 128, 48, TFT_BLACK);

    // drawString targetTemperature and targetHumidity
    tft.drawString("Set T: " + String(targetTemperature, 2) + " °C", 5, 50);
    tft.drawString("Set H: " + String(targetHumidity, 0) + " %", 5, 65);
    // print targetTemperature and targetHumidity to serial monitor with
    // debugMessage()
    debugMessage("Target Temperature: " + String(targetTemperature, 2) + " °C");
    debugMessage("Target Humidity: " + String(targetHumidity, 0) + " %");
}
