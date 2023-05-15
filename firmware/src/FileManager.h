#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <Arduino.h>

void    initializeStorage();

void    writeToFile(const char *fileName, const String &content, bool append = false);

String  readFromFile(const char *fileName);

#endif