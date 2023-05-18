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
    const unsigned long  interval       = 1000 * 60;  // Pause duration

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
                    String(currentTemperature) + "," +
                        String(getMillisecondsSinceEpoch()) + "\n",
                    true);
        writeToFile("/data_hum.csv",
                    String(currentHumidity) + "," +
                        String(getMillisecondsSinceEpoch()) + "\n",
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
    auto    now = std::chrono::system_clock::now();
    int64_t milliseconds =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch())
            .count();

    return milliseconds;
}

void controlHeatElementMosfet(float currentTemperature,
                              float targetTemperature) {
    if (currentTemperature < targetTemperature - hysteresis) {
        digitalWrite(mosfetPin, ON);
    } else if (currentTemperature > targetTemperature + hysteresis) {
        digitalWrite(mosfetPin, OFF);
    }
}

void controlHumidityVentServo(int currentHumidity, int targetHumidity) {
    int humidityError = targetHumidity - currentHumidity;
    if (humidityError > 5) {
        // If the humidity is more than 5% less than the target, open the vent
        ventServo.write(servoOpenPosition);
    } else if (humidityError < -5) {
        // If the humidity is more than 5% greater than the target, close the
        // vent
        ventServo.write(servoClosedPosition);
    } else {
        // If the humidity is within 5% of the target, do nothing
    }
}