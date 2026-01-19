
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
#include "files.h"

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
        debug("Card failed, or not present");
        while (1)
            ;
    }

    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE) {
        debug("No SD card attached");
        return;
    }

    debug("SD Card Type: ");
    if (cardType == CARD_MMC) {
        debug("MMC");
    } else if (cardType == CARD_SD) {
        debug("SDSC");
    } else if (cardType == CARD_SDHC) {
        debug("SDHC");
    } else {
        debug("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    debugf("SD Card Size: %lluMB\n", cardSize);

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
    debugf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    debugf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
}

void loop() {
    //display
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
    if (M5Cardputer.BtnA.wasPressed()) {//wasReleased? wasClicked?
      delay(1000);
    }
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
