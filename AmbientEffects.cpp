#include "AmbientEffects.h"
#include "LedUtils.h"

extern const byte *new_words[];
extern byte new_words_length;
extern const byte *const_words[];
extern byte const_words_length;

void showSimple(CRGB on, CRGB off) {
    fillLeds(off);
    showAllWords(on, new_words, new_words_length);
    showAllWords(on, const_words, const_words_length);
}

static void _fadeall() {
    for (int i = 0; i < 10 * 11; i++) {
        leds[i].nscale8(180);
    }
}

int8_t matrix_worms[11] = {-5, -10, -3, -13, -1, 0, -1, -5, -6, -11, -4};

void ambientMatrix(CRGB on, CRGB off) {
    _fadeall();

    for (byte i = 0; i < 11; i++) {
        if (matrix_worms[i] < 10) {
            setLeds(matrix_worms[i], i, off, 1, false);
        } else if (matrix_worms[i] == 10) {
            matrix_worms[i] = -(int8_t)random8(14);
        }
        matrix_worms[i]++;
    }

    showAllWords(on, new_words, new_words_length);
    showAllWords(on, const_words, const_words_length);
}

void ambientParty(CRGB on, CRGB off) {
    static byte e = 0;
    e++;
    if (e < 3) return;
    e = 0;
    for (int i = 0; i < 110; i++) {
        leds[i] = CHSV(random8(), 255, 180);
    }

    showAllWords(on, new_words, new_words_length);
    showAllWords(on, const_words, const_words_length);
}

void ambientBreathing(CRGB on, CRGB off) {
    static byte phase = 0;
    phase += 3;
    byte val = sin8(phase);
    byte bright = 40 + (val * (byte)215) / 255;
    FastLED.setBrightness(bright);

    fillLeds(off);
    showAllWords(on, new_words, new_words_length);
    showAllWords(on, const_words, const_words_length);
}

void ambientRainbow(CRGB on, CRGB off) {
    static byte hue = 0;
    hue++;
    CRGB rainbow = CHSV(hue, 255, 255);

    fillLeds(off);
    showAllWords(rainbow, new_words, new_words_length);
    showAllWords(rainbow, const_words, const_words_length);
}

void ambientFire(CRGB on, CRGB off) {
    for (int i = 0; i < 110; i++) {
        byte heat = random8(120, 255);
        byte hue = random8(0, 40);
        leds[i] = CHSV(hue, 240, heat);
    }
    for (int i = 1; i < 109; i++) {
        leds[i] = leds[i].lerp8(leds[i - 1], 80);
    }

    showAllWords(on, new_words, new_words_length);
    showAllWords(on, const_words, const_words_length);
}

void ambientTwinkle(CRGB on, CRGB off) {
    for (int i = 0; i < 110; i++) {
        leds[i].nscale8(200);
    }

    for (byte i = 0; i < 3; i++) {
        byte pos = random8(110);
        leds[pos] = CHSV(random8(), 50, random8(100, 255));
    }

    showAllWords(on, new_words, new_words_length);
    showAllWords(on, const_words, const_words_length);
}

int8_t snow_flakes[11] = {-2, -8, -1, -12, -4, -6, -3, -9, -5, -7, -11};

void ambientSnow(CRGB on, CRGB off) {
    for (int i = 0; i < 110; i++) {
        leds[i].nscale8(80);
    }

    for (byte i = 0; i < 11; i++) {
        if (snow_flakes[i] >= 0 && snow_flakes[i] < 10) {
            CRGB snow = CRGB(180, 180, 220);
            setLeds(snow_flakes[i], i, snow, 1, false);
        }
        if (random8() < 100) {
            snow_flakes[i]++;
        }
        if (snow_flakes[i] >= 10) {
            snow_flakes[i] = -(int8_t)random8(5, 20);
        }
    }

    showAllWords(on, new_words, new_words_length);
    showAllWords(on, const_words, const_words_length);
}
