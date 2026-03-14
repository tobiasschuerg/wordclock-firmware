#include "BluetoothHandler.h"
#include "WordClockConfig.h"
#include "WordClockSettings.h"
#include "SnakeGame.h"
#include <SoftwareSerial.h>
#include <DS3232RTC.h>
#include <TimeLib.h>

extern SoftwareSerial btSerial;
extern DS3232RTC myRTC;
extern bool forceTransition;

/**
   Parses and executes the bluetooth commands.
*/
void handleBluetooth() {
    while (btSerial.available() >= 4) {
        byte type = btSerial.read();
        Serial.print("\n~~BT: ");
        Serial.print((char) type);

        switch (type) {
            case 'F': {
                byte red = btSerial.read();
                byte green = btSerial.read();
                byte blue = btSerial.read();
                foreground = CRGB(red, green, blue);
                Serial.print(red); Serial.print(','); Serial.print(green); Serial.print(','); Serial.println(blue);
                break;
            }
            case 'B': {
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
            case 'T': {
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
            case 'D': {
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
            case 'S': {
                btSerial.read();
                btSerial.read();
                btSerial.read();
                storeSettings();
                break;
            }
            case 'P': {
                btSerial.read();
                btSerial.read();
                btSerial.read();
                forceTransition = true;
                break;
            }
            case 'G': {
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
            case 'N': {
                byte subCmd = btSerial.read();
                byte data1 = btSerial.read();
                byte data2 = btSerial.read();
                switch (subCmd) {
                    case 1:
                        if (isNightModeActive) {
                            FastLED.setBrightness(brightness);
                        }
                        snakeStart();
                        Serial.println("Snake started");
                        break;
                    case 2:
                        snakeSetDirection(data1);
                        break;
                    case 0:
                    default:
                        snakeStop();
                        if (isNightModeActive) {
                            FastLED.setBrightness(nightModeBrightness);
                        }
                        forceTransition = true;
                        Serial.println("Snake stopped");
                        break;
                }
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
