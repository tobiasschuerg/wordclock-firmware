#include "TransitionEffects.h"
#include "LedUtils.h"
#include "WordClockConfig.h"

extern const byte *new_words[];
extern byte new_words_length;
extern const byte *old_words[];
extern byte old_words_length;
extern const byte *const_words[];
extern byte const_words_length;

void transitionFade(CRGB on, CRGB off) {
    for (int e = 1; e <= 256 / 8; e++) {
        int i = e * 8 - 1;
        fillLeds(off);
        showAllWords(on, const_words, const_words_length);
        showAllWords(off.lerp8(on, i), new_words, new_words_length);
        showAllWords(on.lerp8(off, i), old_words, old_words_length);
        FastLED.show();
        delay(50);
    }
}

void transitionTypewriter(CRGB on, CRGB off) {
    byte max_old_word = 0;
    byte max_new_word = 0;

    for (byte i = 0; i < old_words_length; i++)
        max_old_word = max(max_old_word, old_words[i][2]);

    for (byte i = 0; i < new_words_length; i++)
        max_new_word = max(max_new_word, new_words[i][2]);

    for (byte i = 0; i <= max_old_word; i++) {
        fillLeds(off);
        showAllWords(on, old_words, old_words_length, 0, 0, 200, i);
        showAllWords(on, const_words, const_words_length);
        FastLED.show();
        delay(180);
    }

    fillLeds(off);
    showAllWords(on, const_words, const_words_length);
    for (byte i = 0; i <= max_new_word; i++) {
        showAllWords(on, new_words, new_words_length, 0, 0, i, 0);
        FastLED.show();
        delay(180);
    }
}

void transitionRollDown(CRGB on, CRGB off) {
    for (int e = 1; e <= 10; e++) {
        fillLeds(off);
        showAllWords(on, const_words, const_words_length, 0, e);
        showAllWords(on, old_words, old_words_length, 0, e);
        FastLED.show();
        delay(80 + (10 - e) * 5);
    }

    for (int e = 10; e >= 0; e--) {
        fillLeds(off);
        showAllWords(on, const_words, const_words_length, 0, -e);
        showAllWords(on, new_words, new_words_length, 0, -e);
        FastLED.show();
        delay(80 + (10 - e) * 8);
    }
}

void transitionWave(CRGB on, CRGB off) {
    for (int radius = 0; radius <= 12; radius++) {
        fillLeds(off);
        showAllWords(on, const_words, const_words_length);
        for (byte w = 0; w < new_words_length; w++) {
            byte row = new_words[w][0];
            byte col = new_words[w][1];
            byte len = new_words[w][2];
            for (byte c = 0; c < len; c++) {
                int dx = (int)(col + c) - 5;
                int dy = (int)row - 5;
                int dist = (dx * dx + dy * dy);
                if (dist <= radius * radius) {
                    setLeds(row, col + c, on, 1, false);
                }
            }
        }
        for (byte w = 0; w < old_words_length; w++) {
            byte row = old_words[w][0];
            byte col = old_words[w][1];
            byte len = old_words[w][2];
            for (byte c = 0; c < len; c++) {
                int dx = (int)(col + c) - 5;
                int dy = (int)row - 5;
                int dist = (dx * dx + dy * dy);
                if (dist > radius * radius) {
                    setLeds(row, col + c, on, 1, false);
                }
            }
        }
        FastLED.show();
        delay(60);
    }
}

void transitionSlide(CRGB on, CRGB off) {
    for (int e = 1; e <= 11; e++) {
        fillLeds(off);
        showAllWords(on, const_words, const_words_length);
        showAllWords(on, old_words, old_words_length, -e, 0);
        FastLED.show();
        delay(50);
    }
    for (int e = 11; e >= 0; e--) {
        fillLeds(off);
        showAllWords(on, const_words, const_words_length);
        showAllWords(on, new_words, new_words_length, e, 0);
        FastLED.show();
        delay(50);
    }
}

void transitionPulse(CRGB on, CRGB off) {
    fillLeds(off);
    showAllWords(on, const_words, const_words_length);
    showAllWords(on, new_words, new_words_length);
    FastLED.setBrightness(255);
    FastLED.show();
    delay(150);

    for (int e = 7; e >= 0; e--) {
        byte b = brightness + (byte)((255 - brightness) * e / 7);
        FastLED.setBrightness(b);
        FastLED.show();
        delay(60);
    }
    FastLED.setBrightness(brightness);
}
