#include "WordClockConfig.h"
#include <EEPROM.h>

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
  EEPROM.write(6, effect);
  EEPROM.write(7, showEsIst);
  EEPROM.write(8, hwVersion);
  // EEPROM.write(9, timezone);
}

/**
   Loads settings from EEPROM.
*/
void loadSettings() {
  foreground.r = EEPROM.read(0);
  foreground.g = EEPROM.read(1);
  foreground.b = EEPROM.read(2);
  background.r = EEPROM.read(3);
  background.g = EEPROM.read(4);
  background.b = EEPROM.read(5);
  effect = EEPROM.read(6);
  showEsIst = EEPROM.read(7);
  hwVersion = EEPROM.read(8);
  // timezone =     EEPROM.read(9);
}
