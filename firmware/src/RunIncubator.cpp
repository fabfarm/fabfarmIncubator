#include "RunIncubator.h"

int number = 0;

void runIncubator() {
    static unsigned long lastCheckSaveData         = 0;
    static unsigned long lastCheckRenewTftData     = 0;
    static unsigned long lastPrintNumberCounter    = 0;
    static unsigned long timeIntervalToPrintNumber = 1000;
    static bool          previousIncubatorStatus   = false;
    bool                 currentIncubatorStatus    = getIncubatorStatus();

    if (currentIncubatorStatus != previousIncubatorStatus) {
        debugMessage("Incubator status has changed: " +
                     String(currentIncubatorStatus ? "active" : "not active"));
        tft.fillScreen(TFT_WHITE);
        tft.setTextColor(TFT_BLACK, TFT_WHITE);
        previousIncubatorStatus = currentIncubatorStatus;
    }

    if (hasIntervalPassed(timeIntervalToSaveData, lastCheckSaveData)) {
        saveData();
    }

    if (hasIntervalPassed(timeIntervalToRenewTftData, lastCheckRenewTftData)) {
        updateTFTDisplay();
    }

    if (!currentIncubatorStatus) {
        pauseSystem();
        return;
    }
    // uncomment this to print number counter on the screen
    // if (hasIntervalPassed(timeIntervalToPrintNumber, lastPrintNumberCounter))
    // {
    //     tft.drawString(String(number), 30, 86, 7);
    //     number++;
    // }

    controlHeatElementMosfet(targetTemperature);
    controlHumidityVentServo(targetHumidity);
    controlTrayServo();
}

void pauseSystem() {
    digitalWrite(mosfetPin, OFF);
    ventServo.write(servoClosedPosition);
    debugMessage("System is Paused");
    tft.fillScreen(TFT_RED);
    // tft.setTextColor(TFT_BLACK, TFT_RED, 1);
    // tft.drawString("SYSTEM IS PAUSED", 60, 60, 7);
    // delay(10000);
}

void saveData() {
    writeToFile("/data.txt",
                String(millis()) + "," + String(bme.readTemperature()) + "," +
                    String(bme.readHumidity()) + "   ",
                true);
    debugMessage("Data saved");
    // debugMessage(readFromFile("/data.txt"));
}

void controlHeatElementMosfet(float targetTemperature) {
    tempInput    = bme.readTemperature();
    tempSetpoint = targetTemperature;
    tempPID.Compute();
    digitalWrite(mosfetPin, tempOutput > 0.5 ? ON : OFF);
    // debugMessage("Heat Element Mosfet controlled");
}

void controlHumidityVentServo(int targetHumidity) {
    humInput    = bme.readHumidity();
    humSetpoint = targetHumidity;
    humPID.Compute();
    int servoPosition = servoClosedPosition - humOutput;
    ventServo.write(servoPosition);
    // debugMessage("Humidity Vent Servo controlled");
}

bool hasIntervalPassed(unsigned long intervalMillis, unsigned long &lastCheck) {
    unsigned long currentMillis = millis();
    if (currentMillis - lastCheck >= intervalMillis) {
        lastCheck = currentMillis;
        return true;
    } else {
        return false;
    }
}

bool getIncubatorStatus() {
    return readFromFile("/setStatus.txt").toInt() == 1;
}
