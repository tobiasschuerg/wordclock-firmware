#include "WordClockConfig.h"
#include "WordClockSettings.h"
#include "LedUtils.h"
#include "AmbientEffects.h"
#include "TransitionEffects.h"
#include "BluetoothHandler.h"
#include "SnakeGame.h"
#include <SoftwareSerial.h>
#include <FastLED.h>
#include <DS3232RTC.h>
#include <TimeLib.h>
#include <Wire.h>

DS3232RTC myRTC;
SoftwareSerial btSerial(8, 9);
CRGB leds[10 * 11 + 4];

/**
   Contains all added words since the last call of generateWords().
*/
const byte *new_words[6];
byte new_words_length = 0;

/**
   Contains all removed words since the last call of generateWords().
*/
const byte *old_words[6];
byte old_words_length = 0;

/**
   Contains all unchanged words since the last call of generateWords().
*/
const byte *const_words[6];
byte const_words_length = 0;

int delayMillis = 550;
time_t lastBt;
bool forceTransition = false;

void printDateTime(time_t t) {
    char buffer[20];
    sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d",
            year(t), month(t), day(t), hour(t), minute(t), second(t));
    Serial.println(buffer);
}

void setup() {
    Serial.begin(9600);

    setSyncProvider(myRTC.get);
    if (timeStatus() != timeSet) {
        Serial.println("Unable to sync with the RTC");
    } else {
        Serial.println("RTC has set the system time");
        printDateTime(now());
    }

    delay(1000);
    FastLED.addLeds<NEOPIXEL, 7>(leds, 10 * 11 + 4);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 1500);
    FastLED.setBrightness(brightness);
    for (int i = 0; i < 10 * 11 + 4; i++) {
        leds[i] = CRGB::Red;
    }
    FastLED.show();
    delay(250);

    loadSettings();

    btSerial.begin(9600);
    lastBt = now();

    delayMillis = 100;
    Serial.println("initialized");
}

void loop() {
    if (snakeGameActive) {
        bool running = snakeUpdate(millis());
        FastLED.show();
        if (!running) {
            btSerial.write('N');
            btSerial.write((byte)0);
            btSerial.write((byte)0);
            btSerial.write((byte)0);
            if (isNightModeActive) {
                FastLED.setBrightness(nightModeBrightness);
            }
        }
        delay(20);
        handleBluetooth();
        return;
    }

    handleNightMode();

    generateWords();

    showCorners(foreground, background);

    bool wordsChanged = (new_words_length > 0 || old_words_length > 0);
    if (wordsChanged) {
        switch (transitionEffect) {
            case 1: transitionFade(foreground, background); break;
            case 2: transitionTypewriter(foreground, background); break;
            case 3: transitionRollDown(foreground, background); break;
            case 4: transitionWave(foreground, background); break;
            case 5: transitionSlide(foreground, background); break;
            case 6: transitionPulse(foreground, background); break;
            default: break;
        }
    }

    switch (ambientEffect) {
        case 1: ambientMatrix(foreground, background); break;
        case 2: ambientParty(foreground, background); break;
        case 3: ambientBreathing(foreground, background); break;
        case 4: ambientRainbow(foreground, background); break;
        case 5: ambientFire(foreground, background); break;
        case 6: ambientTwinkle(foreground, background); break;
        case 7: ambientSnow(foreground, background); break;
        case 0:
        default: showSimple(foreground, background); break;
    }

    FastLED.show();
    delay(delayMillis);

    if (now() - lastBt > 0) {
        handleBluetooth();
        lastBt = now();
    }
}

bool isNightHour(int h) {
    if (nightModeStartHour > nightModeEndtHour) {
        return h >= nightModeStartHour || h < nightModeEndtHour;
    } else {
        return h >= nightModeStartHour && h < nightModeEndtHour;
    }
}

CRGB savedBackground;
byte savedAmbientEffect;
byte savedTransitionEffect;

/**
   During night mode, the light is dimmed and the background turned off.
*/
void handleNightMode() {
    if (isNightModeEnabled) {
        time_t t = myRTC.get();
        int h = hour(t);
        if (!isNightModeActive && isNightHour(h)) {
            savedBackground = background;
            savedAmbientEffect = ambientEffect;
            savedTransitionEffect = transitionEffect;
            background = CRGB(0, 0, 0);
            FastLED.setBrightness(nightModeBrightness);
            ambientEffect = 0;
            transitionEffect = 0;
            isNightModeActive = true;
            Serial.println("Night mode enabled");
        } else if (isNightModeActive && !isNightHour(h)) {
            background = savedBackground;
            ambientEffect = savedAmbientEffect;
            transitionEffect = savedTransitionEffect;
            FastLED.setBrightness(brightness);
            isNightModeActive = false;
            Serial.println("Night mode disabled");
        }
    }
}

/**
   Generates the words to show using the actual time.
*/
void generateWords() {
    time_t local = now();

    clearWords();

    if (showEsIst) {
        addWord(ES);
        addWord(IST);
    }

    byte shour = hourFormat12(local);
    switch (minute(local) / 5) {
        case 0:
            if (shour == 1) shour = 0;
            addWord(UHR);
            break;
        case 1:  addWord(FUENF); addWord(NACH); break;
        case 2:  addWord(ZEHN); addWord(NACH); break;
        case 3:  addWord(VIERTEL); addWord(NACH); break;
        case 4:  addWord(ZWANZIG); addWord(NACH); break;
        case 5:  addWord(FUENF); addWord(VOR); addWord(HALB); shour++; break;
        case 6:  addWord(HALB); shour++; break;
        case 7:  addWord(FUENF); addWord(NACH); addWord(HALB); shour++; break;
        case 8:  addWord(ZWANZIG); addWord(VOR); shour++; break;
        case 9:  addWord(VIERTEL); addWord(VOR); shour++; break;
        case 10: addWord(ZEHN); addWord(VOR); shour++; break;
        case 11: addWord(FUENF); addWord(VOR); shour++; break;
    }

    if (shour == 13) shour = 1;
    addWord(HOUR[shour]);
}

void showCorners(CRGB on, CRGB off) {
    byte num = minute() % 5;
    for (byte i = 0; i < 4; i++) {
        leds[10 * 11 + i] = (i < num) ? on : off;
    }
}

/**
   Adds a word to the new words array. When it was previously in the old words it is added to the constant words.
*/
void addWord(const byte part[]) {
    bool in_old_words = 0;
    for (byte i = 0; i < old_words_length; i++) {
        if (part == old_words[i]) {
            in_old_words = 1;
            const_words[const_words_length] = part;
            const_words_length++;
            old_words_length--;
            for (byte e = i; e < old_words_length; e++) {
                old_words[e] = old_words[e + 1];
            }
            break;
        }
    }

    if (!in_old_words) {
        new_words[new_words_length] = part;
        new_words_length++;
    }
}

/**
   Put all new and constant words to the old words.
   When forceTransition is set, discard old words so all appear as new.
*/
void clearWords() {
    if (forceTransition) {
        old_words_length = 0;
        new_words_length = 0;
        const_words_length = 0;
        forceTransition = false;
        return;
    }

    for (byte i = 0; i < new_words_length; i++) {
        old_words[i] = new_words[i];
    }
    old_words_length = new_words_length;

    for (byte i = 0; i < const_words_length; i++) {
        old_words[old_words_length] = const_words[i];
        old_words_length++;
    }

    new_words_length = 0;
    const_words_length = 0;
}
