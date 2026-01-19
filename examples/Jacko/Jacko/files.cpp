// TODO: Make bettter
#include "files.h"

void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
    debugf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if (!root) {
        debug("Failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        debug("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            Serial.print("  DIR : ");
            debug(file.name());
            if (levels) {
                listDir(fs, file.path(), levels - 1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            debug(String(file.size()).c_str());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char *path) {
    debugf("Creating Dir: %s\n", path);
    if (fs.mkdir(path)) {
        debug("Dir created");
    } else {
        debug("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char *path) {
    debugf("Removing Dir: %s\n", path);
    if (fs.rmdir(path)) {
        debug("Dir removed");
    } else {
        debug("rmdir failed");
    }
}

void readFile(fs::FS &fs, const char *path) {
    debugf("Reading file: %s\n", path);

    File file = fs.open(path);
    if (!file) {
        debug("Failed to open file for reading");
        return;
    }

    debug("Read from file: ");
    // TODO: better ead
    while (file.available()) {
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char *path, const char *message) {
    debugf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if (!file) {
        debug("Failed to open file for writing");
        return;
    }
    if (file.print(message)) {
        debug("File written");
    } else {
        debug("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char *path, const char *message) {
    debugf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if (!file) {
        debug("Failed to open file for appending");
        return;
    }
    if (file.print(message)) {
        debug("Message appended");
    } else {
        debug("Append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char *path1, const char *path2) {
    debugf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        debug("File renamed");
    } else {
        debug("Rename failed");
    }
}

void deleteFile(fs::FS &fs, const char *path) {
    debugf("Deleting file: %s\n", path);
    if (fs.remove(path)) {
        debug("File deleted");
    } else {
       
        debug("Delete failed");
    }
}

void debugf(const char *format, ...) {
    char buf[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buf, 256, format, args);
    va_end(args);
    Serial.print(buf);
    //canvas.printf(buf);
    //canvas.pushSprite(0, 0);
}

void debug(const char *str) {
    Serial.println(str);
    //canvas.println(str);
    //canvas.pushSprite(0, 0);
}
