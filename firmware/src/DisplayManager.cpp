#include "DisplayManager.h"
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "config.h"

void displayLineOnTFT(uint16_t x, uint16_t y, const char* label, float value, const char* unit) {
    tft.setCursor(x, y);
    tft.print(label);
    tft.printf("%.1f", value);
    tft.print(unit);
}

void initializeTFTDisplay() {
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(BLACK);
    tft.loadFont(notoFont);
    tft.setTextSize(2);
    tft.setTextColor(WHITE, BLACK);
    tft.setCursor(0, 0);
    tft.print("INITIALISING TFT...");
  }

void errorWithCode(String errorCode) {
    tft.fillScreen(BLACK);
    tft.setCursor(0, 0);
    tft.print("PLS CHECK DOCS:");
    tft.setCursor(0, 20);
    tft.print("ERROR CODE: " + errorCode);
}

void displayError(const String &errorMessage, const String &errorCode) {
    tft.fillScreen(RED);
    tft.setTextColor(WHITE);
    tft.setCursor(0, 0);
    tft.print(errorMessage);
    if (errorCode != "") {
        tft.setCursor(0, 20);
        tft.print("CODE: " + errorCode);
    }
}

void updateTFTDisplay() {
    if (targetTemperature != -500 || targetHumidity != -500) {
        static float lastTemperature = -501.0, lastHumidity = -501.0, lastPressure = -501.0;

        if (currentTemperature != lastTemperature) {
            tft.fillRect(0, 0, tft.width(), 20, BLACK); // Clear the entire line
            displayLineOnTFT(0, 0, "T: ", currentTemperature, "C");
            lastTemperature = currentTemperature;
        }

        if (currentHumidity != lastHumidity) {
            tft.fillRect(0, 20, tft.width(), 20, BLACK); // Clear the entire line
            displayLineOnTFT(0, 20, "H: ", currentHumidity, "%");
            lastHumidity = currentHumidity;
        }

        if (currentPressure != lastPressure) {
            tft.fillRect(0, 40, tft.width(), 20, BLACK); // Clear the entire line
            displayLineOnTFT(0, 40, "P: ", currentPressure, "hPa");
            lastPressure = currentPressure;
        }

        tft.fillRect(0, 70, tft.width(), 20, BLACK); // Clear the entire line
        displayLineOnTFT(0, 70, "setT:", targetTemperature, "C");

        tft.fillRect(0, 90, tft.width(), 20, BLACK); // Clear the entire line
        displayLineOnTFT(0, 90, "setH:", targetHumidity, "%");
    }
}