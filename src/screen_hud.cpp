#include "screens.h"
#include "display.h"
#include <math.h>
#include <stdlib.h>

static float  _outer_angle = 0.0f;
static float  _inner_angle = 0.0f;
static float  _pulse       = 0.0f;
static float  _scanline    = 0.0f;
static int    _data[4]     = {421, 87, 1024, 33};
static float  _next_tick   = 0.0f;

void screen_hud_update(float t) {
    _outer_angle =  t * 18.0f;  // 18 deg/sec clockwise
    _inner_angle = -t * 30.0f;  // 30 deg/sec counter-clockwise
    _pulse       = 0.5f + 0.5f * sinf(t * 6.0f);
    _scanline    = fmodf(t * 60.0f, 360.0f); // full rotation every 6 sec

    // Randomise data readouts every 0.5 s
    if (t >= _next_tick) {
        _data[0] = 300 + rand() % 200;
        _data[1] = rand() % 100;
        _data[2] = 900 + rand() % 200;
        _data[3] = rand() % 60;
        _next_tick = t + 0.5f;
    }
}

// Draw a segmented arc: NUM_SEGS short arcs with gaps between them
static void draw_segmented_arc(TFT_eSprite& spr, int cx, int cy, int r,
                                float start_deg, int num_segs,
                                float seg_deg, float gap_deg,
                                int thickness, uint16_t color) {
    for (int i = 0; i < num_segs; i++) {
        float a0 = start_deg + i * (seg_deg + gap_deg);
        float a1 = a0 + seg_deg;
        draw_ring_arc(spr, cx, cy, r, thickness, a0, a1, color);
    }
}

void screen_hud_draw(TFT_eSprite& spr) {
    const int cx = 120, cy = 120;
    const uint16_t BLUE  = tft.color565(0, 160, 255);
    const uint16_t LBLUE = tft.color565(0, 220, 255);

    spr.fillSprite(tft.color565(0, 5, 15)); // very dark blue-black

    // Outer segmented ring (12 segments, rotates CW)
    draw_segmented_arc(spr, cx, cy, 108, _outer_angle, 12, 22.0f, 8.0f, 4, BLUE);

    // Inner segmented ring (8 segments, rotates CCW)
    draw_segmented_arc(spr, cx, cy, 78, _inner_angle, 8, 32.0f, 13.0f, 3, LBLUE);

    // Crosshair — pulsing opacity via colour interpolation
    uint8_t cross_v = (uint8_t)(60 + _pulse * 100);
    uint16_t cross_col = tft.color565(0, cross_v, cross_v * 2);
    spr.drawLine(cx - 110, cy, cx + 110, cy, cross_col);
    spr.drawLine(cx, cy - 110, cx, cy + 110, cross_col);
    // Crosshair gaps in centre
    spr.drawFastHLine(cx - 20, cy, 40, tft.color565(0, 5, 15));
    spr.drawFastVLine(cx, cy - 20, 40, tft.color565(0, 5, 15));

    // Scanline sweep (thin radial line)
    float scan_rad = (_scanline - 90.0f) * DEG_TO_RAD;
    int sx = cx + (int)(cosf(scan_rad) * 110);
    int sy = cy + (int)(sinf(scan_rad) * 110);
    spr.drawLine(cx, cy, sx, sy, tft.color565(0, 60, 120));

    // Centre circle
    spr.drawCircle(cx, cy, 18, LBLUE);
    spr.fillCircle(cx, cy, 5, LBLUE);

    // Data readouts at four compass positions (outside inner ring)
    spr.setTextSize(1);
    spr.setTextDatum(MC_DATUM);
    spr.setTextColor(LBLUE, tft.color565(0, 5, 15));
    spr.drawNumber(_data[0], cx,       cy - 50); // top
    spr.drawNumber(_data[1], cx + 50,  cy);       // right
    spr.drawNumber(_data[2], cx,       cy + 50); // bottom
    spr.drawNumber(_data[3], cx - 50,  cy);       // left
}
