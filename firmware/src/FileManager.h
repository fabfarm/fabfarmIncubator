#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <Arduino.h>
#include <FS.h>

void initializeStorage();

void writeToFile(const char *fileName, const String &content,
                 bool append = true);

String   readFromFile(const char *fileName);
fs::File readFile(const char *fileName);

#endif