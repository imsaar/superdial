#include "screens.h"
#include "display.h"
#include <math.h>

// Each ring: radius, angular speed (deg/sec), color (RGB565 set in draw)
struct NeonRing {
    int   radius;
    float speed_dps; // degrees per second
    float offset;    // initial phase offset
};

static const NeonRing RINGS[] = {
    { 30,  90.0f,  0.0f   },
    { 55,  55.0f,  45.0f  },
    { 80,  35.0f,  120.0f },
    { 105, 20.0f,  200.0f },
};
static const int NUM_RINGS = 4;

static float _t_local = 0.0f;

void screen_neon_update(float t) {
    _t_local = t;
}

// Draw a glowing arc: one thick arc + two thinner, brighter overlaid arcs
static void draw_glow_arc(TFT_eSprite& spr, int cx, int cy, int r,
                          float start_deg, float end_deg,
                          uint16_t dim_col, uint16_t bright_col) {
    draw_ring_arc(spr, cx, cy, r,     5, start_deg, end_deg, dim_col);
    draw_ring_arc(spr, cx, cy, r,     3, start_deg, end_deg, bright_col);
    draw_ring_arc(spr, cx, cy, r - 1, 1, start_deg, end_deg, TFT_WHITE);
}

void screen_neon_draw(TFT_eSprite& spr) {
    const int cx = 120, cy = 120;
    spr.fillSprite(TFT_BLACK);

    // Colours: dim (background glow) / bright (core) for each ring
    const uint16_t DIM[]    = {
        tft.color565(0, 80, 80),    // dim cyan
        tft.color565(80, 0, 80),    // dim magenta
        tft.color565(0, 80, 0),     // dim lime
        tft.color565(80, 40, 0),    // dim orange
    };
    const uint16_t BRIGHT[] = {
        tft.color565(0, 255, 255),  // cyan
        tft.color565(255, 0, 255),  // magenta
        tft.color565(0, 255, 80),   // lime
        tft.color565(255, 140, 0),  // orange
    };

    for (int i = 0; i < NUM_RINGS; i++) {
        float angle = _t_local * RINGS[i].speed_dps + RINGS[i].offset;
        // Each ring shows a 200° arc that rotates; gap is 160°
        float arc_start = fmodf(angle, 360.0f);
        float arc_end   = arc_start + 200.0f;
        draw_glow_arc(spr, cx, cy, RINGS[i].radius,
                      arc_start, arc_end, DIM[i], BRIGHT[i]);
    }

    // Tiny centre dot
    spr.fillCircle(cx, cy, 4, TFT_WHITE);
    spr.fillCircle(cx, cy, 2, tft.color565(0, 255, 255));
}
