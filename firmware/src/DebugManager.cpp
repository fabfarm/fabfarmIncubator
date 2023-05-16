#include "DebugManager.h"

void debugMessage(String message) {
  if (debugMode) {
    Serial.println(message);
  }
}