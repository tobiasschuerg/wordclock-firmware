#include "WordClockConfig.h"
#include "WordClockSettings.h"
//#include <Adafruit_GFX.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <FastLED.h>
#include <DS3232RTC.h>  // needs 1.x    //http://github.com/JChristensen/DS3232RTC
#include <TimeLib.h>    //https://github.com/PaulStoffregen/Time
// #include <Timezone.h>     //https://github.com/JChristensen/Timezone
#include <Wire.h>  //http://arduino.cc/en/Reference/Wire (included with Arduino IDE)
//#include <glcdfont.c>

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
    setSyncProvider(RTC.get);
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

    switch (effect) {
        case 1:
            showFade(foreground, background);
            break;
        case 2:
            showTypewriter(foreground, background);
            break;
        case 3:
            showMatrix(foreground, background);
            break;
        case 4:
            showRollDown(foreground, background);
            break;
        case 5:
            showParty(foreground, background);
            break;
        case 6:
            scanner(foreground, background);
            break;
        case 0:
        default:
            showSimple(foreground, background);
            break;
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
byte savedEffect;

void handleNightMode() {
    if (isNightModeEnabled) {
        time_t t = RTC.get();
        int h = hour(t);
        if (!isNightModeActive && isNightHour(h)) {
            savedBackground = background;
            savedEffect = effect;
            background = CRGB(0, 0, 0);                  // turn off background
            FastLED.setBrightness(nightModeBrightness);  // dim the time
            effect = 0;                                  // no effect
            isNightModeActive = true;
            Serial.println("Night mode enabled");
        } else if (isNightModeActive && !isNightHour(h)) {
            background = savedBackground;
            effect = savedEffect;
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
   Fades new words in and old words out.
*/
void showFade(CRGB on, CRGB off) {
    if (new_words_length > 0 || old_words_length > 0) {
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

    showSimple(on, off);
}

/**
   Rolls characters out and in to change display..
*/
void showRollDown(CRGB on, CRGB off) {
    if (new_words_length > 0 || old_words_length > 0) {
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

    showSimple(on, off);
}

/**
   Shows the "matrix effect" in background color and the time in foreground color.
*/
int8_t matrix_worms[11] = {-5, -10, -3, -13, -1, 0, -1, -5, -6, -11, -4};

void showMatrix(CRGB on, CRGB off) {
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
   Removes old words and inserts new words in a typewriter style.
*/
void showTypewriter(CRGB on, CRGB off) {
    if (new_words_length > 0 || old_words_length > 0) {
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

    showSimple(on, off);
}

/**
   Rolls characters out and in to change display..
*/
void showParty(CRGB on, CRGB off) {
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
                Serial.println(foreground);
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
                effect = btSerial.read();
                showEsIst = (btSerial.read() == 1);
                btSerial.read();
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
                RTC.set(now());
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
                RTC.set(now());
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
                        btSerial.write(effect);
                        btSerial.write(showEsIst);
                        btSerial.write('E');
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
                        //            case 'Z':
                        //              btSerial.write('Z');
                        //              btSerial.write(timezone);
                        //              btSerial.write('Z'); btSerial.write('Z');
                        //              break;
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