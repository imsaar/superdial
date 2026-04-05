# GC9A01 Dynamic Dial Showcase — Design Spec
**Date:** 2026-04-05  
**Board:** ESP32-C3 SuperMini  
**Display:** GC9A01 1.28" round TFT (240×240, 65K colors)  
**Toolchain:** PlatformIO CLI

---

## Overview

A fully animated display showcase for the GC9A01 round TFT. Six visually distinct screens cycle automatically every 3 seconds, each with continuous on-screen animation. The goal is to demonstrate the display's color depth, refresh speed, and round canvas to its fullest.

---

## Hardware

| Signal | ESP32-C3 GPIO |
|--------|---------------|
| SCL (SPI CLK) | GPIO 0 |
| SDA (SPI MOSI) | GPIO 1 |
| DC | GPIO 2 |
| CS | GPIO 3 |
| RST | GPIO 4 |

- No BLK pin on this module (backlight always on)
- PlatformIO board target: `seeed_xiao_esp32c3`
- Display library: `TFT_eSPI` configured for GC9A01 driver

---

## Architecture

**Option B — Screen array with function pointer pairs.**

Each screen exposes exactly two functions:
```cpp
void screen_X_update(float t);       // advance animation state; t = elapsed seconds
void screen_X_draw(TFT_eSprite& spr); // render into sprite
```

A `screens[]` array in `main.cpp` holds these pairs. The main loop:
1. Calls `screen_update(active, t)` every frame to advance animation
2. Calls `screen_draw(active, spr)` to render into a 240×240 sprite
3. Pushes the sprite to the display in one DMA transfer (~30 fps, flicker-free)
4. Every 3 seconds, increments `active` (wraps at 6) to switch screens

### Why sprites?
TFT_eSPI sprites act as an off-screen framebuffer. The full frame is composited in RAM then pushed atomically — eliminating tearing and partial-frame flicker during fast animation.

---

## Project Structure

```
superdial/
├── platformio.ini
└── src/
    ├── main.cpp               # setup(), loop(), screen cycling timer
    ├── display.h/.cpp         # GC9A01 init, TFT_eSPI config, color helpers
    ├── screen_clock.cpp       # Screen 1: Analog clock
    ├── screen_speedometer.cpp # Screen 2: Racing speedometer
    ├── screen_rainbow.cpp     # Screen 3: Rainbow arc gauge
    ├── screen_neon.cpp        # Screen 4: Neon concentric rings
    ├── screen_hud.cpp         # Screen 5: Sci-fi HUD
    └── screen_vumeter.cpp     # Screen 6: Retro VU meter
```

---

## The 6 Screens

### 1. Analog Clock
- Dark gradient circular dial face
- Hour, minute, second hands rendered with anti-aliased lines
- Colored tick marks (major = white, minor = grey)
- Second hand sweeps smoothly in real time using `millis()`
- Center cap dot in accent color

### 2. Speedometer
- Arc spans ~240° around the dial (classic automotive layout)
- Needle oscillates from 0 to 220 and back on a ~4s sine cycle
- Arc color transitions green → yellow → red as speed increases
- Large numeric readout in the lower center counts up/down with the needle
- Speed label ("km/h") in small text below the number

### 3. Rainbow Arc Gauge
- Full HSV spectrum arc rendered around the full 360° of the dial
- A bright white value marker sweeps around the arc continuously
- Center fill breathes from near-black to dark-grey in sync
- Thin inner ring outlines the dial in gold

### 4. Neon Rings
- Four concentric rings, each a distinct neon color: cyan, magenta, lime, orange
- Each ring rotates at a different angular speed (creates organic motion)
- Glow effect achieved by drawing 3 progressively thinner/brighter lines per ring
- Black background makes colors pop maximally

### 5. Sci-Fi HUD
- Two segmented arcs (inner and outer) rotate in opposite directions
- Crosshair lines pulse in opacity
- Four corner "data" numbers tick up/down randomly every 0.5s
- Scanline sweep rotates around the center
- Color palette: electric blue + white on near-black

### 6. Retro VU Meter
- 20 vertical bars arranged in a fan/arc pattern to fit the round canvas
- Bars animate to a simulated sine-wave audio signal with per-bar phase offsets
- Color gradient: green (low) → yellow (mid) → red (peak)
- Peak-hold dots sit above each bar, decay slowly
- "VU" label centered at top in retro amber

---

## Rendering Pipeline

```
loop()
  │
  ├── update: screen_X_update(elapsed_t)   ← advances animation state
  │
  ├── draw:   screen_X_draw(sprite)        ← renders to 240×240 sprite
  │
  └── push:   sprite.pushSprite(0, 0)      ← DMA transfer to GC9A01
```

Frame budget at 30fps = ~33ms. Each screen must complete update+draw within this window. TFT_eSPI's hardware SPI on the C3 at 40MHz gives ~8ms for the push, leaving ~25ms for drawing logic.

---

## platformio.ini

```ini
[env:seeed_xiao_esp32c3]
platform = espressif32
board = seeed_xiao_esp32c3
framework = arduino
lib_deps =
    bodmer/TFT_eSPI
build_flags =
    -DUSER_SETUP_LOADED=1
    -DGFX_BL=-1
    -DGC9A01_DRIVER=1
    -DTFT_WIDTH=240
    -DTFT_HEIGHT=240
    -DTFT_MOSI=1
    -DTFT_SCLK=0
    -DTFT_CS=3
    -DTFT_DC=2
    -DTFT_RST=4
    -DSPI_FREQUENCY=40000000
```

---

## Success Criteria

- All 6 screens render without flicker or tearing
- Screen switches every 3 seconds, seamlessly
- Each screen is visually distinct and fully animated during its dwell time
- Frame rate stays at or above 25fps for all screens
- Build compiles cleanly with PlatformIO CLI (`pio run`)
