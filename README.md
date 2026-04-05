# SuperDial — Animated Display Showcase for GC9A01

A 6-screen fully animated display showcase running on an **ESP32-C3 SuperMini** with a **GC9A01 1.28" round TFT** (240x240, 65K colors). Screens cycle automatically every 3 seconds, demonstrating the display's color depth, refresh speed, and round canvas.

## Screens

| # | Screen | Description |
|---|--------|-------------|
| 1 | **Analog Clock** | Sweeping hour/minute/second hands on a dark blue dial with colored tick marks |
| 2 | **Speedometer** | Automotive-style arc gauge with an oscillating needle (0-220 km/h), green-to-red color fill |
| 3 | **Rainbow Arc** | Full HSV spectrum ring with a sweeping white marker and breathing background |
| 4 | **Neon Rings** | Four concentric glowing arcs (cyan, magenta, lime, orange) rotating at different speeds |
| 5 | **Sci-Fi HUD** | Rotating segmented arcs, pulsing crosshair, scanline sweep, and ticking data readouts |
| 6 | **Retro VU Meter** | 20 radial bars bouncing to simulated audio with peak-hold dots and green/yellow/red gradient |

All screens render into a 240x240 off-screen sprite (TFT_eSPI) and push the full frame via SPI in one transfer for flicker-free ~30fps animation.

## Hardware

### Components

| Component | Model |
|-----------|-------|
| MCU | ESP32-C3 SuperMini |
| Display | GC9A01 1.28" Round TFT (240x240, SPI) |

### Wiring

| Display Pin | ESP32-C3 GPIO | Function |
|-------------|---------------|----------|
| SCL | GPIO 5 | SPI Clock |
| SDA | GPIO 6 | SPI MOSI (data) |
| DC | GPIO 7 | Data/Command select |
| CS | GPIO 8 | Chip Select |
| RST | GPIO 9 | Reset |
| VCC | 3.3V | Power |
| GND | GND | Ground |

> **GPIO 0-4 are reserved** on ESP32-C3 — they are connected to the internal SPI flash bus and cannot be used for peripherals.

## Software

### Prerequisites

- [PlatformIO CLI](https://docs.platformio.org/en/latest/core/installation.html) (or PlatformIO IDE extension for VS Code)

### Build & Flash

```bash
# Build
pio run

# Flash (hold BOOT button on the SuperMini during upload)
pio run --target upload

# Serial monitor
pio device monitor
```

> **Upload tip:** The ESP32-C3 SuperMini uses built-in USB CDC. If upload fails, hold the **BOOT** button while running the upload command and release once "Connecting..." appears.

### Project Structure

```
superdial/
├── platformio.ini              # Board, library, pin config, build flags
├── src/
│   ├── main.cpp                # Setup, render loop, 3-second screen cycling
│   ├── display.h / display.cpp # TFT + sprite init, HSV color helper, arc drawing utility
│   ├── screens.h               # Screen struct interface, forward declarations
│   ├── screen_clock.cpp        # Screen 1: Analog clock
│   ├── screen_speedometer.cpp  # Screen 2: Speedometer
│   ├── screen_rainbow.cpp      # Screen 3: Rainbow arc gauge
│   ├── screen_neon.cpp         # Screen 4: Neon concentric rings
│   ├── screen_hud.cpp          # Screen 5: Sci-fi HUD
│   └── screen_vumeter.cpp      # Screen 6: Retro VU meter
└── docs/
    └── superpowers/
        ├── specs/              # Design specification
        └── plans/              # Implementation plan
```

### Architecture

Each screen exposes two functions registered in a `screens[]` array:

```cpp
struct Screen {
    void (*update)(float t);          // advance animation; t = seconds since screen activated
    void (*draw)(TFT_eSprite& spr);   // render into 240x240 sprite
    const char* name;
};
```

The main loop:
1. Calls `update(t)` to advance animation state
2. Calls `draw(spr)` to render into the off-screen sprite
3. Pushes the sprite to the display via `spr.pushSprite(0, 0)`
4. Switches to the next screen every 3 seconds

### Adding a New Screen

1. Create `src/screen_myscreen.cpp` with `screen_myscreen_update(float t)` and `screen_myscreen_draw(TFT_eSprite& spr)`
2. Add forward declarations to `src/screens.h`
3. Add the entry to `screens[]` in `src/main.cpp` and increment `SCREEN_COUNT`

## ESP32-C3 SuperMini + TFT_eSPI Known Issue

TFT_eSPI v2.5.43 has a bug on ESP32-C3: `SPI_PORT` is set to `SPI2_HOST` (enum value `1`), but ESP-IDF's `REG_SPI_BASE(i)` expects register index `2` for SPI2. This causes `SPI_USER_REG` to resolve to address `0x10` (null pointer), crashing in `tft.init()` with a `Store access fault`.

**Fix applied in this project:** After running `pio run` for the first time (which downloads TFT_eSPI), edit:

```
.pio/libdeps/esp32c3_supermini/TFT_eSPI/Processors/TFT_eSPI_ESP32_C3.h
```

Change line ~71:
```cpp
// Before:
#define SPI_PORT SPI2_HOST
// After:
#define SPI_PORT 2
```

This fix is required each time `.pio/libdeps/` is cleaned or the library is reinstalled.

## Dependencies

| Library | Version | Purpose |
|---------|---------|---------|
| [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) | 2.5.43 | Display driver + sprite engine |
| espressif32 | 6.9.0 | PlatformIO platform (Arduino ESP32 core v2.x) |

## License

MIT
