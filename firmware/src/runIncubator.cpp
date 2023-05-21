#include "runIncubator.h"

#include "DebugManager.h"
#include "DisplayManager.h"
#include "FileManager.h"
#include "SensorManager.h"
#include "ServoManager.h"
#include "WebServerManager.h"
#include "config.h"

void runIncubator() {
    bool isIncubatorActive = getIncubatorStatus();
    debugMessage("Incubator is: " +
                 String(isIncubatorActive ? "active" : "not active"));

    if (!isIncubatorActive) {
        pauseSystem();
        return;
    }

    if (!checkAndSetSensorValues()) {
        return;
    }

    if (hasIntervalPassed(5000)) {
        saveData();
    }

    controlHeatElementMosfet(currentTemperature, targetTemperature);
    controlHumidityVentServo(currentHumidity, targetHumidity);
    controlTrayServo();
    updateTFTDisplay();
}

void pauseSystem() {
    tft.fillScreen(BLACK);
    tft.setCursor(0, 0);
    tft.print("SYSTEM PAUSED");
    digitalWrite(mosfetPin, OFF);
    ventServo.write(servoClosedPosition);
    debugMessage("System is Paused");
}

bool checkAndSetSensorValues() {
    currentTemperature = bme.readTemperature();
    currentHumidity    = bme.readHumidity();
    currentPressure    = bme.readPressure() / 100.0F;

    if (isnan(currentHumidity) || isnan(currentTemperature)) {
        debugMessage("Failed to read from sensor!");
        return false;
    }

    if (currentTemperature < minTemperature ||
        currentTemperature > maxTemperature || currentHumidity < minHumidity ||
        currentHumidity > maxHumidity) {
        debugMessage("Sensor values out of range!");
        return false;
    }
    debugMessage("Sensor values set");
    return true;
}

void saveData() {
    writeToFile("/data_temp.csv",
                String(millis()) + "," + String(currentTemperature) + "\n",
                true);
    writeToFile("/data_hum.csv",
                String(millis()) + "," + String(currentHumidity) + "\n", true);
    debugMessage("Data saved to SPIFFS");
}

void controlHeatElementMosfet(float currentTemperature,
                              float targetTemperature) {
    tempInput    = currentTemperature;
    tempSetpoint = targetTemperature;
    tempPID.Compute();
    digitalWrite(mosfetPin, tempOutput > 0.5 ? ON : OFF);
    debugMessage("Heat Element Mosfet controlled");
}

void controlHumidityVentServo(int currentHumidity, int targetHumidity) {
    humInput    = currentHumidity;
    humSetpoint = targetHumidity;
    humPID.Compute();
    int servoPosition = servoClosedPosition - humOutput;
    ventServo.write(servoPosition);
    debugMessage("Humidity Vent Servo controlled");
}

// Function to check if the specified number of milliseconds has passed
bool hasIntervalPassed(unsigned long intervalMillis) {
    // Declare a static variable to remember the last time we checked
    static unsigned long lastCheck = 0;

    unsigned long currentMillis = millis();
    if (currentMillis - lastCheck >= intervalMillis) {
        lastCheck = currentMillis;
        return true;
    } else {
        return false;
    }
}
