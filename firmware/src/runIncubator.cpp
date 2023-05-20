#include "runIncubator.h"

#include "DebugManager.h"
#include "DisplayManager.h"
#include "FileManager.h"
#include "SensorManager.h"
#include "ServoManager.h"
#include "WebServerManager.h"
#include "config.h"

void runIncubator() {
    static unsigned long previousMillis = 0;
    const unsigned long  interval       = 5000;  // Pause duration

    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
        // Pause duration has passed, perform actions here

        bool isIncubatorActive = getIncubatorStatus();
        debugMessage("Incubator is: " +
                     String(isIncubatorActive ? "active" : "not active"));
        if (!isIncubatorActive) {
            tft.fillScreen(BLACK);
            tft.setCursor(0, 0);
            tft.print("SYSTEM PAUSED");
            digitalWrite(mosfetPin, OFF);
            ventServo.write(servoClosedPosition);
            return;
        }
        currentTemperature = bme.readTemperature();
        currentHumidity    = bme.readHumidity();
        currentPressure    = bme.readPressure() / 100.0F;
        if (isnan(currentHumidity) || isnan(currentTemperature)) {
            debugMessage("Failed to read from sensor!");
            return;
        }
        if (currentTemperature < minTemperature ||
            currentTemperature > maxTemperature ||
            currentHumidity < minHumidity || currentHumidity > maxHumidity) {
            debugMessage("Sensor values out of range!");
            return;
        }
        writeToFile("/data_temp.csv",
                    String(getMillisecondsSinceEpoch()) + "," +
                        String(currentTemperature) + "\n",
                    true);
        writeToFile("/data_hum.csv",
                    String(getMillisecondsSinceEpoch()) + "," +
                        String(currentHumidity) + "\n",
                    true);
        debugMessage("Data saved to SPIFFS");
        controlHeatElementMosfet(currentTemperature, targetTemperature);
        controlHumidityVentServo(currentHumidity, targetHumidity);
        controlTrayServo();
        updateTFTDisplay();

        // Reset the timer
        previousMillis = currentMillis;
    }
}

int getMillisecondsSinceEpoch() {

    unsigned long milliseconds = millis();

    return milliseconds;
}

void controlHeatElementMosfet(float currentTemperature, float targetTemperature) {
    tempInput = currentTemperature; // Set the current temperature
    tempSetpoint = targetTemperature; // Set the target temperature
    tempPID.Compute(); // Compute the new output
    // If the output is greater than 0.5, turn the heating element on
    // Otherwise, turn it off
    if (tempOutput > 0.5) {
        digitalWrite(mosfetPin, ON);
    } else {
        digitalWrite(mosfetPin, OFF);
    }
}

void controlHumidityVentServo(int currentHumidity, int targetHumidity) {

    humInput = currentHumidity;
    humSetpoint = targetHumidity;
    humPID.Compute();
    int servoPosition = servoClosedPosition - humOutput;
    ventServo.write(servoPosition);
}