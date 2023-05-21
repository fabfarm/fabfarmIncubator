#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>

#define notoFont NotoSansBold15

void initializeTFTDisplay();

void errorWithCode(String errorCode);

void displayLineOnTFT(uint16_t x, uint16_t y, const char* label, float value,
                      const char* unit);

void displayError(const String& errorMessage, const String& errorCode = "");

void updateTFTDisplay();

void updateLineIfChanged(uint16_t y, const char* label, float& lastValue,
                         float currentValue, const char* unit);

void displayTargetValue(uint16_t y, const char* label, float value,
                        const char* unit);

#endif