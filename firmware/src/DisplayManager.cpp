#include "DisplayManager.h"
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "config.h"

constexpr int16_t defaultXPos = 0;
constexpr int16_t lineHeight = 20;
constexpr float invalidValue = -500.0;

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

void updateLineIfChanged(uint16_t y, const char* label, float &lastValue, float currentValue, const char* unit) {
    if (currentValue != lastValue) {
        tft.fillRect(defaultXPos, y, tft.width(), lineHeight, BLACK); // Clear the entire line
        displayLineOnTFT(defaultXPos, y, label, currentValue, unit);
        lastValue = currentValue;
    }
}

void displayTargetValue(uint16_t y, const char* label, float value, const char* unit) {
    tft.fillRect(defaultXPos, y, tft.width(), lineHeight, BLACK); // Clear the entire line
    displayLineOnTFT(defaultXPos, y, label, value, unit);
}

void updateTFTDisplay() {
    if (targetTemperature != invalidValue || targetHumidity != invalidValue) {
        static float lastTemperature = invalidValue + 1.0, lastHumidity = invalidValue + 1.0, lastPressure = invalidValue + 1.0;

        updateLineIfChanged(0, "T: ", lastTemperature, currentTemperature, "C");
        updateLineIfChanged(lineHeight, "H: ", lastHumidity, currentHumidity, "%");
        updateLineIfChanged(lineHeight * 2, "P: ", lastPressure, currentPressure, "hPa");

        // Always display target values (assuming they can be changed)
        displayTargetValue(lineHeight * 3.5, "setT:", targetTemperature, "C");
        displayTargetValue(lineHeight * 4.5, "setH:", targetHumidity, "%");
    }
}

