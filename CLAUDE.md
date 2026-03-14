# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Arduino Nano-based German word clock using WS2812B/Neopixel RGB LEDs (114 LEDs: 10×11 grid + 4 corner minute LEDs), DS3232RTC for timekeeping, and HC-05 Bluetooth for wireless configuration via a binary serial protocol.

## Build & Upload

This is an **Arduino IDE** project (not PlatformIO). The Arduino Nano uses the **old bootloader** (ATmega328P old bootloader).

### Via Arduino IDE
Open `Wordclock.ino`, select board "Arduino Nano", processor "ATmega328P (Old Bootloader)", and upload via USB.

### Via CLI (arduino-cli)
```bash
# The bundled arduino-cli from Arduino IDE 2.x:
ARDUINO_CLI="/c/Program Files/Arduino IDE/resources/app/lib/backend/resources/arduino-cli.exe"

# Compile
"$ARDUINO_CLI" compile --fqbn arduino:avr:nano:cpu=atmega328old "Wordclock"

# Upload (adjust COM port as needed)
"$ARDUINO_CLI" upload --fqbn arduino:avr:nano:cpu=atmega328old --port COM3 "Wordclock"
```

Note: Using `cpu=atmega328` (new bootloader) will fail with "not in sync" errors. Always use `cpu=atmega328old`.

**Required external libraries:**
- FastLED (LED control)
- DS3232RTC v2.x (real-time clock) — code uses an explicit `DS3232RTC myRTC` instance (v2.x removed the global `RTC` object)
- TimeLib (Paul Stoffregen's time library)

Built-in libraries used: Wire, SoftwareSerial, EEPROM.

## Architecture

**Wordclock.ino** — Main sketch with `setup()`/`loop()`, night mode handling, and German word generation from the current time.

**LedUtils.h/.cpp** — Low-level LED helpers: `setLeds()` (handles hardware wiring differences), `fillLeds()`, and `showAllWords()` overloads. Used by all effect modules and SnakeGame.

**AmbientEffects.h/.cpp** — 7 ambient effects (continuous background animations) plus `showSimple()` (solid display). Each ambient function manages its own static state.

**TransitionEffects.h/.cpp** — 6 transition effects that play once when displayed words change.

**BluetoothHandler.h/.cpp** — Bluetooth protocol parser (`handleBluetooth()`) and `PROTOCOL_VERSION`. Processes 4-byte commands from the HC-05 module.

**WordClockConfig.h/.cpp** — German word definitions as LED position arrays `[row, col, length]`, global state variables (colors, brightness, effect selection, night mode params), and default values.

**WordClockSettings.h/.cpp** — EEPROM persistence (addresses 0-8): foreground/background colors, effect, showEsIst flag, hardware version. Two functions: `storeSettings()` and `loadSettings()`.

**SnakeGame.h/.cpp** — Snake game playable over Bluetooth. Uses `LedUtils` for LED access.

### Effects

Ambient and transition effects are independently configurable. Ambient effects run every frame (background animation), transition effects play once when displayed words change.

**Ambient Effects (IDs 0-7):**

| ID | Name | Description |
|----|------|-------------|
| 0 | None | Solid background color, no animation |
| 1 | Matrix | Digital rain in background color (default) |
| 2 | Party | Random rainbow noise |
| 3 | Breathing | Sine-wave brightness pulsing |
| 4 | Rainbow | Text color cycles through HSV hue wheel |
| 5 | Fire | Warm flickering candle-light |
| 6 | Twinkle | Random background LEDs sparkle |
| 7 | Snow | White pixels fall down columns |

**Transition Effects (IDs 0-6):**

| ID | Name | Description |
|----|------|-------------|
| 0 | None | Instant word change (default: Fade) |
| 1 | Fade | Words crossfade in/out (~1.6s) |
| 2 | Typewriter | Characters appear/disappear sequentially |
| 3 | RollDown | Words vertically scroll in/out with easing |
| 4 | Wave | Circular ripple reveals new words from center |
| 5 | Slide | Old words slide left, new words slide in from right |
| 6 | Pulse | Words flash bright on change, then settle |

## Hardware Pin Mapping

- LED data: pin 7 (FastLED NEOPIXEL)
- Bluetooth: pins 8 (RX) / 9 (TX) at 9600 baud
- RTC: I2C (A4/A5)

## Bluetooth Protocol

Single-character commands followed by 3 data bytes: `F` (foreground RGB), `B` (background RGB), `E` (ambientEffect, transitionEffect, showEsIst), `T` (time h/m/s), `D` (date d/m/y), `G` (query, sub-commands: F/B/E/T/D/V), `S` (save to EEPROM). `GV` returns the protocol version (currently 1); the app checks this on connect and rejects mismatches.

## Protocol Versioning

`PROTOCOL_VERSION` (in `BluetoothHandler.h`) must be incremented on any breaking protocol change. When incrementing:

1. Update `PROTOCOL_VERSION` in `BluetoothHandler.h`
2. Update `PROTOCOL_VERSION` in the Android app's `BluetoothMessageService.kt`
3. Tag both repos: `git tag v<N>`

## Key Details

- Two hardware versions with different LED wiring directions (controlled by `hwVersion`)
- Night mode dims to brightness 50, disables background and effects (default 23:00–06:00)
- German time convention: "halb eins" = 12:30 (half toward next hour)
- Corner LEDs display minutes remainder (0-4) beyond the 5-minute word resolution
