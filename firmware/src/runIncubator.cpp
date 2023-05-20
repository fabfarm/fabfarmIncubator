#include "runIncubator.h"

#include "DebugManager.h"
#include "DisplayManager.h"
#include "FileManager.h"
#include "SensorManager.h"
#include "ServoManager.h"
#include "WebServerManager.h"
#include "config.h"

void runIncubator() {
    // Check the status of the incubator
    bool isIncubatorActive = getIncubatorStatus();
    debugMessage("Incubator is: " + String(isIncubatorActive ? "active" : "not active"));

    // If the incubator is not active, pause the system
    if (!isIncubatorActive) {
        tft.fillScreen(BLACK);
        tft.setCursor(0, 0);
        tft.print("SYSTEM PAUSED");
        digitalWrite(mosfetPin, OFF);
        ventServo.write(servoClosedPosition);
        return;
    }
    
    // Get the current temperature from the BME sensor
    currentTemperature = bme.readTemperature();
    currentHumidity    = bme.readHumidity();
    currentPressure    = bme.readPressure() / 100.0F;

    // If the temperature or humidity reading is not a number, print an error message and exit the function
    if (isnan(currentHumidity) || isnan(currentTemperature)) {
        debugMessage("Failed to read from sensor!");
        return;
    }
    
    // If the temperature or humidity is outside the acceptable range, print an error message and exit the function
    if (currentTemperature < minTemperature ||
        currentTemperature > maxTemperature ||
        currentHumidity < minHumidity || currentHumidity > maxHumidity) {
        debugMessage("Sensor values out of range!");
        return;
    }
    
    // Only save data to file if 5 seconds have passed
    if (hasIntervalPassed(5000)) {
        // Save the current temperature and timestamp to a file
        writeToFile("/data_temp.csv",
                    String(millis()) + "," +
                        String(currentTemperature) + "\n",
                    true);

        // Save the current humidity and timestamp to a file
        writeToFile("/data_hum.csv",
                    String(millis()) + "," +
                        String(currentHumidity) + "\n",
                    true);

        // Print a debug message indicating the data has been saved
        debugMessage("Data saved to SPIFFS");
    }
    
    // Adjust the heat element using the mosfet based on the current and target temperatures
    controlHeatElementMosfet(currentTemperature, targetTemperature);
    
    // Adjust the vent servo based on the current and target humidity
    controlHumidityVentServo(currentHumidity, targetHumidity);
    
    // Perform the tray servo operation
    controlTrayServo();
    
    // Update the display
    updateTFTDisplay();
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
