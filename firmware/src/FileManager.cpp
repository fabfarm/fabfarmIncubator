#include "FileManager.h"

void initializeStorage() {
    if (!SPIFFS.begin(true)) {
        debugMessage("An Error has occurred while mounting SPIFFS");
        return;
    }
    debugMessage("SPIFFS mounted successfully!");
}

void writeToFile(const char *fileName, const String &content, bool append) {
    debugMessage(String("Writing content to ") + fileName);
    fs::File file = SPIFFS.open(fileName, append ? "a" : "w");
    if (!file) {
        debugMessage(String("Error opening ") + fileName + " for writing");
        return;
    }
    file.print(content);
    file.close();
}

String readFromFile(const char *fileName) {
    if (!SPIFFS.exists(fileName)) {
        debugMessage(String("Error opening ") + fileName + " for reading");
        return "";
    }
    fs::File file    = SPIFFS.open(fileName, "r");
    String   content = file.readString();
    file.close();
    return content;
}

fs::File readFile(const char *fileName) {
    if (!SPIFFS.exists(fileName)) {
        debugMessage(String("Error opening ") + fileName + " for reading");
        return fs::File();
    }
    fs::File file = SPIFFS.open(fileName, "r");
    file.close();
    return file;
}