#include "pidManager.h"

void setupPidControllers() {
    tempPID.SetMode(AUTOMATIC);
    tempPID.SetOutputLimits(0, 255);
    humPID.SetMode(AUTOMATIC);
    humPID.SetOutputLimits(0, 200);
}