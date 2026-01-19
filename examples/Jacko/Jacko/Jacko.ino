
/**
 * @file Jacko.ino
 * @author Jacko (jackokring@gmail.com)
 * @brief My app
 * @version 0.1
 * @date 2026-01-19
 *
 *
 * @Hardwares: M5Cardputer
 * @Platform Version: Arduino M5Stack Board Manager v3.2.5
 * @Dependent Library:
 * M5GFX: https://github.com/m5stack/M5GFX
 * M5Unified: https://github.com/m5stack/M5Unified
 */

#include "M5Cardputer.h"

#include <SPI.h>
#include <SD.h>

#define SD_SPI_SCK_PIN  40
#define SD_SPI_MISO_PIN 39
#define SD_SPI_MOSI_PIN 14
#define SD_SPI_CS_PIN   12

void listDir(fs::FS &fs, const char *dirname, uint8_t levels);
void createDir(fs::FS &fs, const char *path);
void removeDir(fs::FS &fs, const char *path);
void readFile(fs::FS &fs, const char *path);
void writeFile(fs::FS &fs, const char *path, const char *message);
void appendFile(fs::FS &fs, const char *path, const char *message);
void renameFile(fs::FS &fs, const char *path1, const char *path2);
void deleteFile(fs::FS &fs, const char *path);
void testFileIO(fs::FS &fs, const char *path);

void printf_log(const char *format, ...);
void println_log(const char *str);

M5Canvas canvas(&M5Cardputer.Display);

void draw_function(LovyanGFX* gfx) {
    int x      = rand() % gfx->width();
    int y      = rand() % gfx->height();
    int r      = (gfx->width() >> 4) + 2;
    uint16_t c = rand();
    gfx->fillRect(x - r, y - r, r * 2, r * 2, c);
}

void setup() {
    auto cfg = M5.config();
    M5Cardputer.begin(cfg, true);
    int textsize = M5Cardputer.Display.height() / 60;
    if (textsize == 0) {
        textsize = 1;
    }
    M5Cardputer.Display.setTextSize(textsize);
    
    // SD Card Initialization
    SPI.begin(SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_SPI_CS_PIN);

    if (!SD.begin(SD_SPI_CS_PIN, SPI, 25000000)) {
        // Print a message if the SD card initialization
        // fails orif the SD card does not exist.
        // 如果SD卡初始化失败或者SD卡不存在，则打印消息.
        println_log("Card failed, or not present");
        while (1)
            ;
    }

    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE) {
        println_log("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC) {
        println_log("MMC");
    } else if (cardType == CARD_SD) {
        println_log("SDSC");
    } else if (cardType == CARD_SDHC) {
        println_log("SDHC");
    } else {
        println_log("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    printf_log("SD Card Size: %lluMB\n", cardSize);

    listDir(SD, "/", 0);
    createDir(SD, "/mydir");
    listDir(SD, "/", 0);
    removeDir(SD, "/mydir");
    listDir(SD, "/", 2);
    writeFile(SD, "/hello.txt", "Hello ");
    appendFile(SD, "/hello.txt", "World!\n");
    readFile(SD, "/hello.txt");
    deleteFile(SD, "/foo.txt");
    renameFile(SD, "/hello.txt", "/foo.txt");
    readFile(SD, "/foo.txt");
    //testFileIO(SD, "/test.txt");
    printf_log("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    printf_log("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
}

void loop() {
    int x      = rand() % M5Cardputer.Display.width();
    int y      = rand() % M5Cardputer.Display.height();
    int r      = (M5Cardputer.Display.width() >> 4) + 2;
    uint16_t c = rand();
    M5Cardputer.Display.fillCircle(x, y, r, c);
    draw_function(&M5Cardputer.Display);
    //buzz
    M5Cardputer.Speaker.tone(10000, 100);
    delay(1000);
    M5Cardputer.Speaker.tone(4000, 20);
    delay(1000);
    //G0
    M5Cardputer.update();
    if (M5Cardputer.BtnA.wasPressed()) {//wasReleased?
      delay(1000);
    }
}

void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
    printf_log("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if (!root) {
        println_log("Failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        println_log("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            Serial.print("  DIR : ");
            println_log(file.name());
            if (levels) {
                listDir(fs, file.path(), levels - 1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            println_log(String(file.size()).c_str());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char *path) {
    printf_log("Creating Dir: %s\n", path);
    if (fs.mkdir(path)) {
        println_log("Dir created");
    } else {
        println_log("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char *path) {
    printf_log("Removing Dir: %s\n", path);
    if (fs.rmdir(path)) {
        println_log("Dir removed");
    } else {
        println_log("rmdir failed");
    }
}

void readFile(fs::FS &fs, const char *path) {
    printf_log("Reading file: %s\n", path);

    File file = fs.open(path);
    if (!file) {
        println_log("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while (file.available()) {
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char *path, const char *message) {
    printf_log("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if (!file) {
        println_log("Failed to open file for writing");
        return;
    }
    if (file.print(message)) {
        println_log("File written");
    } else {
        println_log("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char *path, const char *message) {
    printf_log("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if (!file) {
        println_log("Failed to open file for appending");
        return;
    }
    if (file.print(message)) {
        println_log("Message appended");
    } else {
        println_log("Append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char *path1, const char *path2) {
    printf_log("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        println_log("File renamed");
    } else {
        println_log("Rename failed");
    }
}

void deleteFile(fs::FS &fs, const char *path) {
    printf_log("Deleting file: %s\n", path);
    if (fs.remove(path)) {
        println_log("File deleted");
    } else {
       
        println_log("Delete failed");
    }
}

void printf_log(const char *format, ...) {
    char buf[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buf, 256, format, args);
    va_end(args);
    Serial.print(buf);
    //canvas.printf(buf);
    //canvas.pushSprite(0, 0);
}

void println_log(const char *str) {
    Serial.println(str);
    //canvas.println(str);
    //canvas.pushSprite(0, 0);
}

/*  if (M5Cardputer.Keyboard.isChange()) {
        if (M5Cardputer.Keyboard.isPressed()) {
            Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

            for (auto i : status.word) {
                data += i;
            }

            if (status.del) {
                data.remove(data.length() - 1);
            }

            if (status.enter) {
                data.remove(0, 2);
                canvas.println(data);
                canvas.pushSprite(4, 4);
                data = "> ";
            }

            M5Cardputer.Display.fillRect(0, M5Cardputer.Display.height() - 28,
                                         M5Cardputer.Display.width(), 25,
                                         BLACK);

            M5Cardputer.Display.drawString(data, 4,
                                           M5Cardputer.Display.height() - 24);
        }
    } */
