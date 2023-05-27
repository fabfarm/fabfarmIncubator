#include "DisplayManager.h"

void initializeTFTDisplay() {
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_WHITE);
    tft.setTextSize(2);
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
    tft.setCursor(20, 20);
    tft.print("INITIALISING TFT...");
}
