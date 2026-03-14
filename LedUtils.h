#ifndef LED_UTILS_H
#define LED_UTILS_H

#include <FastLED.h>

extern CRGB leds[];

void setLeds(int y, int x, CRGB color, int len, bool add);
void fillLeds(CRGB off);
void showAllWords(CRGB color, const byte *wds[], byte wds_length);
void showAllWords(CRGB color, const byte *wds[], byte wds_length, char xadd, char yadd);
void showAllWords(CRGB color, const byte *wds[], byte wds_length, char xadd, char yadd, byte maxlen, byte cut);

#endif // LED_UTILS_H
