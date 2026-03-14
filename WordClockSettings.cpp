#include "WordClockConfig.h"
#include <EEPROM.h>

#define EEPROM_MAGIC_ADDR 9
#define EEPROM_MAGIC_VALUE 0xA6  // bumped from 0xA5 to reset old single-effect layout

/**
   Stores settings to EEPROM.
*/
void storeSettings() {
    EEPROM.write(0, foreground.r);
    EEPROM.write(1, foreground.g);
    EEPROM.write(2, foreground.b);
    EEPROM.write(3, background.r);
    EEPROM.write(4, background.g);
    EEPROM.write(5, background.b);
    EEPROM.write(6, ambientEffect);
    EEPROM.write(7, showEsIst);
    EEPROM.write(8, hwVersion);
    EEPROM.write(EEPROM_MAGIC_ADDR, EEPROM_MAGIC_VALUE);
    EEPROM.write(10, transitionEffect);
}

/**
   Loads settings from EEPROM. Uses defaults if EEPROM is uninitialized.
*/
void loadSettings() {
    if (EEPROM.read(EEPROM_MAGIC_ADDR) != EEPROM_MAGIC_VALUE) {
        Serial.println("EEPROM uninitialized, using defaults");
        return;
    }
    foreground.r = EEPROM.read(0);
    foreground.g = EEPROM.read(1);
    foreground.b = EEPROM.read(2);
    background.r = EEPROM.read(3);
    background.g = EEPROM.read(4);
    background.b = EEPROM.read(5);
    ambientEffect = EEPROM.read(6);
    showEsIst = EEPROM.read(7);
    hwVersion = EEPROM.read(8);
    transitionEffect = EEPROM.read(10);
}
