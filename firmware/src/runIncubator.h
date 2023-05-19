#ifndef RUN_INCUBATOR_h
#define RUN_INCUBATOR_h

void runIncubator();

void controlHumidityVentServo(int currentHumidity, int targetHumidity);
int  getMillisecondsSinceEpoch();
void controlHeatElementMosfet(float currentTemperature,
                              float targetTemperature);

#endif