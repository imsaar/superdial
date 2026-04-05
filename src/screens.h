#pragma once
#include <TFT_eSPI.h>

struct Screen {
    void (*update)(float t);   // advance animation state; t = seconds since screen became active
    void (*draw)(TFT_eSprite& spr); // render into sprite (do NOT call pushSprite here)
    const char* name;
};

// ── Screen 1: Analog Clock ──────────────────────────────────────────────────
void screen_clock_update(float t);
void screen_clock_draw(TFT_eSprite& spr);

// ── Screen 2: Speedometer ───────────────────────────────────────────────────
void screen_speedometer_update(float t);
void screen_speedometer_draw(TFT_eSprite& spr);

// ── Screen 3: Rainbow Arc ───────────────────────────────────────────────────
void screen_rainbow_update(float t);
void screen_rainbow_draw(TFT_eSprite& spr);

// ── Screen 4: Neon Rings ────────────────────────────────────────────────────
void screen_neon_update(float t);
void screen_neon_draw(TFT_eSprite& spr);

// ── Screen 5: Sci-Fi HUD ────────────────────────────────────────────────────
void screen_hud_update(float t);
void screen_hud_draw(TFT_eSprite& spr);

// ── Screen 6: Retro VU Meter ────────────────────────────────────────────────
void screen_vumeter_update(float t);
void screen_vumeter_draw(TFT_eSprite& spr);

extern Screen screens[];
extern const int SCREEN_COUNT;
