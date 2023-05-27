#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <Arduino.h>
#include <FS.h>

#include "DebugManager.h"
#include "DisplayManager.h"
#include "WebServerManager.h"

void initializeStorage();

void writeToFile(const char *fileName, const String &content,
                 bool append = true);

String   readFromFile(const char *fileName);
fs::File openFileForReading(const char *fileName);
#endif