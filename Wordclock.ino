#include "WordClockConfig.h"
#include "WordClockSettings.h"
//#include <Adafruit_GFX.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <FastLED.h>
#include <DS3232RTC.h>  //http://github.com/JChristensen/DS3232RTC
#include <TimeLib.h>    //https://github.com/PaulStoffregen/Time
#include <Wire.h>  //http://arduino.cc/en/Reference/Wire (included with Arduino IDE)

const byte PROTOCOL_VERSION = 1;

DS3232RTC myRTC;
SoftwareSerial btSerial(8, 9);
CRGB leds[10 * 11 + 4];

/**
   Contains all added words since  the last call of generateWords().
*/
const byte *new_words[6];
byte new_words_length = 0;

/**
   Contains all removed words since the last call of generateWords().
*/
const byte *old_words[6];
byte old_words_length = 0;

/*
   Contains all unchanged words since the last call of generateWords()
*/
const byte *const_words[6];
byte const_words_length = 0;

int delayMillis = 550;
time_t lastBt;

void printDateTime(time_t t) {
    char buffer[20];
    sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d",
            year(t), month(t), day(t), hour(t), minute(t), second(t));
    Serial.println(buffer);
}

void setup() {
    Serial.begin(9600);

    // sync arduino with the real time clock
    setSyncProvider(myRTC.get);
    if (timeStatus() != timeSet) {
        Serial.println("Unable to sync with the RTC");
    } else {
        Serial.println("RTC has set the system time");
        printDateTime(now());  // print current time
    }

    //setup FastLED
    delay(1000);  // power-up safety delay
    FastLED.addLeds<NEOPIXEL, 7>(leds, 10 * 11 + 4);
    FastLED.setBrightness(brightness);
    // show all red
    for (int i = 0; i < 10 * 11 + 4; i++) {
        leds[i] = CRGB::Red;
    }
    FastLED.show();
    delay(250);

    loadSettings();

    // start bluetooth
    btSerial.begin(9600);
    lastBt = now();

    delayMillis = 100;
    Serial.println("initialized");
}

void loop() {
    handleNightMode();

    generateWords();

    showCorners(foreground, background);

    // Run transition animation when words change
    bool wordsChanged = (new_words_length > 0 || old_words_length > 0);
    if (wordsChanged) {
        switch (transitionEffect) {
            case 1: transitionFade(foreground, background); break;
            case 2: transitionTypewriter(foreground, background); break;
            case 3: transitionRollDown(foreground, background); break;
            case 4: transitionWave(foreground, background); break;
            case 5: transitionSlide(foreground, background); break;
            case 6: transitionPulse(foreground, background); break;
            default: break;  // 0 = no transition
        }
    }

    // Run ambient effect every frame
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

    if (now() - lastBt > 0) {  // only check bluetooth at most once per second
        handleBluetooth();
        lastBt = now();
    }
}

/**
 * During night mode (configurable in the config), the light is dimmed and the background turned off.
 */
bool isNightHour(int h) {
    if (nightModeStartHour > nightModeEndtHour) {
        // e.g. 23:00 - 06:00 (spans midnight)
        return h >= nightModeStartHour || h < nightModeEndtHour;
    } else {
        return h >= nightModeStartHour && h < nightModeEndtHour;
    }
}

CRGB savedBackground;
byte savedAmbientEffect;
byte savedTransitionEffect;

void handleNightMode() {
    if (isNightModeEnabled) {
        time_t t = myRTC.get();
        int h = hour(t);
        if (!isNightModeActive && isNightHour(h)) {
            savedBackground = background;
            savedAmbientEffect = ambientEffect;
            savedTransitionEffect = transitionEffect;
            background = CRGB(0, 0, 0);                  // turn off background
            FastLED.setBrightness(nightModeBrightness);  // dim the time
            ambientEffect = 0;                           // no ambient effect
            transitionEffect = 0;                        // no transition effect
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
        case 1:
            addWord(FUENF);
            addWord(NACH);
            break;
        case 2:
            addWord(ZEHN);
            addWord(NACH);
            break;
        case 3:
            addWord(VIERTEL);
            addWord(NACH);
            break;
        case 4:
            addWord(ZWANZIG);
            addWord(NACH);
            break;
        case 5:
            addWord(FUENF);
            addWord(VOR);
            addWord(HALB);
            shour++;
            break;
        case 6:
            addWord(HALB);
            shour++;
            break;
        case 7:
            addWord(FUENF);
            addWord(NACH);
            addWord(HALB);
            shour++;
            break;
        case 8:
            addWord(ZWANZIG);
            addWord(VOR);
            shour++;
            break;
        case 9:
            addWord(VIERTEL);
            addWord(VOR);
            shour++;
            break;
        case 10:
            addWord(ZEHN);
            addWord(VOR);
            shour++;
            break;
        case 11:
            addWord(FUENF);
            addWord(VOR);
            shour++;
            break;
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
   Shows all the new and constant words.
*/
void showSimple(CRGB on, CRGB off) {
    fillLeds(off);
    showAllWords(on, new_words, new_words_length);
    showAllWords(on, const_words, const_words_length);
}

/**
   Transition: fades new words in and old words out.
*/
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

/**
   Transition: rolls characters out and in to change display.
*/
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

/**
   Ambient: "matrix effect" digital rain in background color.
*/
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

void _fadeall() {
    for (int i = 0; i < 10 * 11; i++) {
        leds[i].nscale8(180);  //180
    }
}


int column = 0;
int row = 0;
time_t scannerLastTime = 0;

void scanner(CRGB on, CRGB off) {

    column++;
    if (column > 10) {
        column = 0;
        row++;
        row = row % 10;
    }

    if (row == 0 && column == 0) {
        fillLeds(off);
        int duration = now() - scannerLastTime;
        Serial.print("Duration(sec): ");
        Serial.println(duration);
        if (duration > 60) {
            delayMillis = delayMillis - 50;
        } else {
            delayMillis = delayMillis + 50;
        }
        Serial.println(delayMillis);
        scannerLastTime = now();
    }
    setLeds(row, column, on, 1, false);
}

/**
   Transition: removes old words and inserts new words in a typewriter style.
*/
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

/**
   Ambient: psychedelic random color noise.
*/
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

/**
   Transition: circular ripple reveals new words from center outward.
*/
void transitionWave(CRGB on, CRGB off) {
    // Ripple outward from center (row 4.5, col 5)
    for (int radius = 0; radius <= 12; radius++) {
        fillLeds(off);
        showAllWords(on, const_words, const_words_length);
        // Show new words only where within radius
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
        // Fade out old words outside radius
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

/**
   Transition: old words slide out left, new words slide in from right.
*/
void transitionSlide(CRGB on, CRGB off) {
    // Slide old words out to the left
    for (int e = 1; e <= 11; e++) {
        fillLeds(off);
        showAllWords(on, const_words, const_words_length);
        showAllWords(on, old_words, old_words_length, -e, 0);
        FastLED.show();
        delay(50);
    }
    // Slide new words in from the right
    for (int e = 11; e >= 0; e--) {
        fillLeds(off);
        showAllWords(on, const_words, const_words_length);
        showAllWords(on, new_words, new_words_length, e, 0);
        FastLED.show();
        delay(50);
    }
}

/**
   Ambient: gentle sine-wave brightness pulsing.
*/
void ambientBreathing(CRGB on, CRGB off) {
    static byte phase = 0;
    phase += 3;
    // sin8 returns 0-255, map to brightness range 40-255
    byte val = sin8(phase);
    byte bright = 40 + (val * (byte)215) / 255;
    FastLED.setBrightness(bright);

    fillLeds(off);
    showAllWords(on, new_words, new_words_length);
    showAllWords(on, const_words, const_words_length);
}

/**
   Ambient: text color cycles through the HSV hue wheel.
*/
void ambientRainbow(CRGB on, CRGB off) {
    static byte hue = 0;
    hue++;
    CRGB rainbow = CHSV(hue, 255, 255);

    fillLeds(off);
    showAllWords(rainbow, new_words, new_words_length);
    showAllWords(rainbow, const_words, const_words_length);
}

/**
   Ambient: warm flickering candle-light background.
*/
void ambientFire(CRGB on, CRGB off) {
    for (int i = 0; i < 110; i++) {
        // Random warm tones: red-orange-yellow
        byte heat = random8(120, 255);
        byte hue = random8(0, 40);  // 0=red, 40=orange-yellow
        leds[i] = CHSV(hue, 240, heat);
    }
    // Smooth it slightly by blending with previous frame
    for (int i = 1; i < 109; i++) {
        leds[i] = leds[i].lerp8(leds[i - 1], 80);
    }

    showAllWords(on, new_words, new_words_length);
    showAllWords(on, const_words, const_words_length);
}

/**
   Ambient: random background LEDs sparkle briefly.
*/
void ambientTwinkle(CRGB on, CRGB off) {
    // Fade all LEDs slightly each frame
    for (int i = 0; i < 110; i++) {
        leds[i].nscale8(200);
    }

    // Light up 2-3 random LEDs per frame
    for (byte i = 0; i < 3; i++) {
        byte pos = random8(110);
        leds[pos] = CHSV(random8(), 50, random8(100, 255));
    }

    showAllWords(on, new_words, new_words_length);
    showAllWords(on, const_words, const_words_length);
}

/**
   Ambient: white pixels fall down columns at varying speeds.
*/
int8_t snow_flakes[11] = {-2, -8, -1, -12, -4, -6, -3, -9, -5, -7, -11};

void ambientSnow(CRGB on, CRGB off) {
    // Fade existing pixels
    for (int i = 0; i < 110; i++) {
        leds[i].nscale8(150);
    }

    // Advance each snowflake
    for (byte i = 0; i < 11; i++) {
        if (snow_flakes[i] >= 0 && snow_flakes[i] < 10) {
            CRGB snow = CRGB(180, 180, 220);
            setLeds(snow_flakes[i], i, snow, 1, false);
        }
        // Variable speed: some columns advance faster
        if (random8() < 180) {
            snow_flakes[i]++;
        }
        if (snow_flakes[i] >= 10) {
            snow_flakes[i] = -(int8_t)random8(5, 20);
        }
    }

    showAllWords(on, new_words, new_words_length);
    showAllWords(on, const_words, const_words_length);
}

/**
   Transition: words briefly flash brighter when they change, then settle.
*/
void transitionPulse(CRGB on, CRGB off) {
    // Flash bright
    fillLeds(off);
    showAllWords(on, const_words, const_words_length);
    showAllWords(on, new_words, new_words_length);
    FastLED.setBrightness(255);
    FastLED.show();
    delay(150);

    // Fade back to normal brightness over 8 steps
    for (int e = 7; e >= 0; e--) {
        byte b = brightness + (byte)((255 - brightness) * e / 7);
        FastLED.setBrightness(b);
        FastLED.show();
        delay(60);
    }
    FastLED.setBrightness(brightness);
}

/**
   Parses and executes the bluetooh commands.
*/
void handleBluetooth() {
    while (btSerial.available() >= 4) {
        byte type = btSerial.read();
        Serial.print("\n~~BT: ");
        Serial.print((char) type);

        switch (type) {
            case 'F': {  //foreground color
                byte red = btSerial.read();
                byte green = btSerial.read();
                byte blue = btSerial.read();
                foreground = CRGB(red, green, blue);
                Serial.print(red); Serial.print(','); Serial.print(green); Serial.print(','); Serial.println(blue);
                break;
            }
            case 'B': {  //background color
                byte red = btSerial.read();
                byte green = btSerial.read();
                byte blue = btSerial.read();
                background = CRGB(red, green, blue);
                break;
            }
            case 'E':
                ambientEffect = btSerial.read();
                transitionEffect = btSerial.read();
                showEsIst = (btSerial.read() == 1);
                break;
            case 'T': {  //time
                byte h = btSerial.read();
                byte m = btSerial.read();
                byte s = btSerial.read();

                Serial.print("Setting time to ");
                Serial.print(h);
                Serial.print(':');
                Serial.print(m);
                Serial.print(':');
                Serial.println(s);

                setTime(h, m, s, day(), month(), year());
                myRTC.set(now());
                break;
            }
            case 'D': {  //date
                byte d = btSerial.read();
                byte m = btSerial.read();
                byte y = btSerial.read();
                Serial.print("Setting date to ");
                Serial.print(d);
                Serial.print('-');
                Serial.print(m);
                Serial.print('-');
                Serial.println(y);
                setTime(hour(), minute(), second(), d, m, y);
                myRTC.set(now());
                break;
            }
                //      case 'Z': { //timeZone
                //          timezone = btSerial.read();
                //          btSerial.read(); btSerial.read();
                //          break;
                //        }
            case 'S': {
                btSerial.read();
                btSerial.read();
                btSerial.read();
                storeSettings();
                break;
            }
            case 'G': {  //get
                byte toGet = btSerial.read();
                Serial.println((char) toGet);
                switch (toGet) {
                    case 'F':
                        btSerial.write('F');
                        btSerial.write(foreground.r);
                        btSerial.write(foreground.g);
                        btSerial.write(foreground.b);
                        break;
                    case 'B':
                        btSerial.write('B');
                        btSerial.write(background.r);
                        btSerial.write(background.g);
                        btSerial.write(background.b);
                        break;
                    case 'E':
                        btSerial.write('E');
                        btSerial.write(ambientEffect);
                        btSerial.write(transitionEffect);
                        btSerial.write(showEsIst);
                        break;
                    case 'T':
                        btSerial.write('T');
                        btSerial.write(hour());
                        btSerial.write(minute());
                        btSerial.write(second());
                        break;
                    case 'D':
                        btSerial.write('D');
                        btSerial.write(day());
                        btSerial.write(month());
                        btSerial.write((byte)(year() % 100));
                        break;
                    case 'V':
                        btSerial.write('V');
                        btSerial.write(PROTOCOL_VERSION);
                        btSerial.write((byte)0);
                        btSerial.write((byte)0);
                        break;
                }
                btSerial.read();
                btSerial.read();
                break;
            }
            default: {
                Serial.print("Unknown command ");
                Serial.println((char) type);
                break;
            }
        }
    }
}

/**
   Adds a word to the new words array. When it is was previously in the new words it is added to the constant words.
*/
void addWord(const byte part[]) {
    bool in_old_words = 0;
    for (byte i = 0; i < old_words_length; i++) {
        if (part == old_words[i]) {
            in_old_words = 1;

            const_words[const_words_length] = part;
            const_words_length++;

            //remove in old_words
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
*/
void clearWords() {
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

/**
   Set all Leds to a single color.
*/
void fillLeds(CRGB off) {
    setLeds(0, 0, off, 11 * 10, false);
}

/**
   Shows a array of words in a specific color.
*/
void showAllWords(CRGB color, const byte *wds[], byte wds_length) {
    showAllWords(color, wds, wds_length, 0, 0, 200, 0);
}

/**
   Shows a array of words in a specific color.
*/
void showAllWords(CRGB color, const byte *wds[], byte wds_length, char xadd, char yadd) {
    showAllWords(color, wds, wds_length, xadd, yadd, 200, 0);
}

/**
   Shows a array of words in a specific color.
   @param maxlen determines the max length of every word.
   @param cut cuts the x last characters of every word
*/
void showAllWords(CRGB color, const byte *wds[], byte wds_length, char xadd, char yadd, byte maxlen, byte cut) {
    for (byte i = 0; i < wds_length; i++) {
        setLeds(wds[i][0] + yadd, wds[i][1] + xadd, color, min(max(wds[i][2] - cut, 0), maxlen), false);
    }
}

/**
   Sets a number of leds starting at x, y to the a specific color.
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