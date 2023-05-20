#ifndef RUN_INCUBATOR_h
#define RUN_INCUBATOR_h

void runIncubator();

void controlHumidityVentServo(int currentHumidity, int targetHumidity);
void controlHeatElementMosfet(float currentTemperature, float targetTemperature);
bool hasIntervalPassed(unsigned long intervalMillis);


#endif