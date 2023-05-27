#include "ServoManager.h"

void connectServos() {
    ventServo.attach(humidityVentServoPin);
    trayServo.attach(trayServoPin);
}

void controlTrayServo() {
    static unsigned long lastTurnTime       = 0;
    static bool          trayServoDirection = true;

    // Read the interval and angle from the SD card
    int trayServoTurnInterval, trayServoTurnAngle;
    trayServoTurnInterval = readFromFile("/trayServoInterval.txt").toFloat();
    trayServoTurnAngle    = readFromFile("/trayServoTurnAngle.txt").toFloat();

    // If we have waited long enough...
    if (millis() - lastTurnTime >= trayServoTurnInterval) {
        // Turn the servo
        int currentServoPosition = trayServo.read();
        int trayServoNewPosition;

        if (trayServoDirection) {
            trayServoNewPosition = currentServoPosition + trayServoTurnAngle;
        } else {
            trayServoNewPosition = currentServoPosition - trayServoTurnAngle;
        }
        trayServo.write(trayServoNewPosition);

        // Reset the timer
        lastTurnTime = millis();

        // Toggle direction
        trayServoDirection = !trayServoDirection;
    }
}