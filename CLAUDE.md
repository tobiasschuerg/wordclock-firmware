# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Arduino Nano-based German word clock using WS2812B/Neopixel RGB LEDs (114 LEDs: 10×11 grid + 4 corner minute LEDs), DS3232RTC for timekeeping, and HC-05 Bluetooth for wireless configuration via a binary serial protocol.

## Build & Upload

This is an **Arduino IDE** project (not PlatformIO). Open `Wordclock.ino` in Arduino IDE, select board "Arduino Nano", and upload via USB.

**Required external libraries:**
- FastLED (LED control)
- DS3232RTC v1.x (real-time clock)
- TimeLib (Paul Stoffregen's time library)

Built-in libraries used: Wire, SoftwareSerial, EEPROM.

## Architecture

**Wordclock.ino** — Main sketch with `setup()`/`loop()`. Contains all display logic: word generation from current time, 6 visual effects (simple, fade, typewriter, matrix, rolldown, party), night mode handling, Bluetooth command processing, and LED rendering via FastLED.

**WordClockConfig.h/.cpp** — German word definitions as LED position arrays `[row, col, length]`, global state variables (colors, brightness, effect selection, night mode params), and default values.

**WordClockSettings.h/.cpp** — EEPROM persistence (addresses 0-8): foreground/background colors, effect, showEsIst flag, hardware version. Two functions: `storeSettings()` and `loadSettings()`.

## Hardware Pin Mapping

- LED data: pin 7 (FastLED NEOPIXEL)
- Bluetooth: pins 8 (RX) / 9 (TX) at 9600 baud
- RTC: I2C (A4/A5)

## Bluetooth Protocol

Single-character commands followed by 3 data bytes: `F` (foreground RGB), `B` (background RGB), `E` (effect + showEsIst), `T` (time h/m/s), `D` (date d/m/y), `G` (query), `S` (save to EEPROM).

## Key Details

- Two hardware versions with different LED wiring directions (controlled by `hwVersion`)
- Night mode dims to brightness 50, disables background and effects (default 23:00–06:00)
- German time convention: "halb eins" = 12:30 (half toward next hour)
- Corner LEDs display minutes remainder (0-4) beyond the 5-minute word resolution
