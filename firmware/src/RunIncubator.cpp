#include "RunIncubator.h"

void runIncubator() {
    bool isIncubatorActive = getIncubatorStatus();
    debugMessage("Incubator is: " +
                 String(isIncubatorActive ? "active" : "not active"));

    if (!isIncubatorActive) {
        pauseSystem();
        return;
    }

    if (hasIntervalPassed(5000)) {
        saveData();
    }

    controlHeatElementMosfet(currentTemperature, targetTemperature);
    controlHumidityVentServo(currentHumidity, targetHumidity);
    controlTrayServo();
}

void pauseSystem() {
    digitalWrite(mosfetPin, OFF);
    ventServo.write(servoClosedPosition);
    debugMessage("System is Paused");
}

void saveData() {
    writeToFile("/data.txt",
                String(millis()) + "," + String(currentTemperature) + "," +
                    String(currentHumidity) + "   ",
                true);
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

bool hasIntervalPassed(unsigned long intervalMillis) {
    static unsigned long lastCheck = 0;

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
