Wordclock
=========

Arduino Nano code for a WS2812B/Neopixel German word clock with 114 RGB LEDs (10x11 grid + 4 corner minute LEDs), DS3232 RTC for timekeeping, and HC-05 Bluetooth for wireless configuration via an Android app.

## Build & Upload

Requires Arduino IDE 2.x with these libraries installed: **FastLED**, **DS3232RTC** (v2.x), **TimeLib**.

The Nano uses the **old bootloader**. In Arduino IDE, select: Board "Arduino Nano", Processor "ATmega328P (Old Bootloader)".

### CLI Upload

```bash
ARDUINO_CLI="/c/Program Files/Arduino IDE/resources/app/lib/backend/resources/arduino-cli.exe"

# Compile and upload (adjust COM port as needed)
"$ARDUINO_CLI" compile --fqbn arduino:avr:nano:cpu=atmega328old Wordclock
"$ARDUINO_CLI" upload --fqbn arduino:avr:nano:cpu=atmega328old --port COM3 Wordclock
```

## HC-05 Bluetooth Configuration

```
AT+CLASS=1
AT+ROLE=0           // Slave
AT+NAME=Wordclock-<NAME>
AT+PSWD=1234
```
