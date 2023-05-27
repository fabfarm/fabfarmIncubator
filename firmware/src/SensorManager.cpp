#include "SensorManager.h"

void initializeSensors() {
    I2CBME.begin(BME_SDA, BME_SCL);
    if (!bme.begin(0x77, &I2CBME)) {
        debugMessage("Could not find a valid BME280 sensor, check wiring!");
        while (1) {
        }
    }
}
