#include "LedUtils.h"
#include "WordClockConfig.h"

/**
   Sets a number of leds starting at x, y to a specific color.
   When add is true the color is added to the existing color at the positions.
*/
void setLeds(int y, int x, CRGB color, int len, bool add) {
    if (hwVersion == 2) y = 9 - y;
    if (y < 0 || y > 9) return;
    int start_led = !(y % 2) ? y * 11 + x : y * 11 + (11 - x) - 1;
    int dir = !(y % 2) ? 1 : -1;
    if (hwVersion == 1) dir = -dir;

    for (int i = 0; i < len; i++) {
        int idx = start_led + i * dir;
        if (idx < 0 || idx >= 10 * 11) continue;
        if (add) {
            leds[idx] += color;
        } else {
            leds[idx] = color;
        }
    }
}

/**
   Set all Leds to a single color.
*/
void fillLeds(CRGB off) {
    setLeds(0, 0, off, 11 * 10, false);
}

/**
   Shows an array of words in a specific color.
*/
void showAllWords(CRGB color, const byte *wds[], byte wds_length) {
    showAllWords(color, wds, wds_length, 0, 0, 200, 0);
}

/**
   Shows an array of words in a specific color with position offset.
*/
void showAllWords(CRGB color, const byte *wds[], byte wds_length, char xadd, char yadd) {
    showAllWords(color, wds, wds_length, xadd, yadd, 200, 0);
}

/**
   Shows an array of words in a specific color.
   @param maxlen determines the max length of every word.
   @param cut cuts the x last characters of every word
*/
void showAllWords(CRGB color, const byte *wds[], byte wds_length, char xadd, char yadd, byte maxlen, byte cut) {
    for (byte i = 0; i < wds_length; i++) {
        setLeds(wds[i][0] + yadd, wds[i][1] + xadd, color, min(max(wds[i][2] - cut, 0), maxlen), false);
    }
}
