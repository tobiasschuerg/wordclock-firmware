Wordclock Firmware
==================

Arduino Nano firmware for a WS2812B/Neopixel German word clock with 114 RGB LEDs (10x11 grid + 4 corner minute LEDs), DS3232 RTC for timekeeping, and HC-05 Bluetooth for wireless configuration via an Android app.

## Protocol Version

The firmware and Android app communicate over a versioned binary Bluetooth protocol. The current protocol version is **1** (defined as `PROTOCOL_VERSION` in `BluetoothHandler.h`).

The app queries the firmware version on connect (`GV` command) and rejects mismatches. When making breaking protocol changes:

1. Increment `PROTOCOL_VERSION` in `BluetoothHandler.h`
2. Increment `PROTOCOL_VERSION` in the Android app's `BluetoothMessageService.kt`
3. Tag both repos: `git tag v<N>` (matching the new version number)

## Effects

Ambient and transition effects are independently configurable.

**Ambient** (continuous background animation):

| ID | Name | Description |
|----|------|-------------|
| 0 | None | Solid background color |
| 1 | Matrix | Digital rain (default) |
| 2 | Party | Random rainbow noise |
| 3 | Breathing | Sine-wave brightness pulsing |
| 4 | Rainbow | Text color cycles through hue wheel |
| 5 | Fire | Warm flickering candle-light |
| 6 | Twinkle | Random LEDs sparkle |
| 7 | Snow | White pixels fall down columns |

**Transition** (plays when displayed words change):

| ID | Name | Description |
|----|------|-------------|
| 0 | None | Instant word change |
| 1 | Fade | Words crossfade in/out |
| 2 | Typewriter | Characters appear/disappear sequentially |
| 3 | RollDown | Words scroll vertically with easing |
| 4 | Wave | Circular ripple from center |
| 5 | Slide | Old words slide left, new from right |
| 6 | Pulse | Words flash bright, then settle |

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

## Bluetooth Protocol

All commands are 4 bytes: 1-byte ASCII command + 3 data bytes.

| Cmd | Data bytes | Description |
|-----|-----------|-------------|
| `F` | R, G, B | Set foreground color |
| `B` | R, G, B | Set background color |
| `E` | ambient, transition, showEsIst | Set effects and "ES IST" toggle |
| `T` | hour, minute, second | Set time |
| `D` | day, month, year-2000 | Set date |
| `S` | 0, 0, 0 | Save settings to EEPROM |
| `G` | sub-cmd, 0, 0 | Query (sub: F/B/E/T/D/V) |

## HC-05 Bluetooth Configuration

```
AT+CLASS=1
AT+ROLE=0           // Slave
AT+NAME=Wordclock-<NAME>
AT+PSWD=1234
```
