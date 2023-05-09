#ifndef WORD_CLOCK_CONFIG_H
#define WORD_CLOCK_CONFIG_H

#include <FastLED.h>

extern const byte ES[];
extern const byte IST[];
extern const byte FUENF[];
extern const byte ZEHN[];
extern const byte ZWANZIG[];
extern const byte VIERTEL[];
extern const byte NACH[];
extern const byte VOR[];
extern const byte HALB[];
extern const byte UHR[];

extern const byte HOUR[][3];

extern CRGB foreground;
extern CRGB background;

extern byte effect;
/**
   Hardware version.
*/
extern byte hwVersion;
/**
   Should "Es ist" be showed?
*/
extern bool showEsIst;

#endif // WORD_CLOCK_CONFIG_H
