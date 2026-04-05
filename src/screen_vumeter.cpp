#include "screens.h"
#include "display.h"
#include <Arduino.h>
#include <math.h>

static const int NUM_BARS = 20;
// Peak hold: height per bar (0.0–1.0)
static float _peak[NUM_BARS];
static float _bar[NUM_BARS];

void screen_vumeter_update(float t) {
    // Simulated audio: sum of a few sine waves with per-bar phase offsets
    for (int i = 0; i < NUM_BARS; i++) {
        float phase = (float)i / NUM_BARS * 2.0f * (float)M_PI;
        float val  = 0.5f + 0.4f * sinf(t * 3.5f + phase)
                          + 0.1f * sinf(t * 7.1f + phase * 1.7f);
        val = constrain(val, 0.0f, 1.0f);
        _bar[i] = val;

        // Peak hold: latch at max, decay slowly
        if (val > _peak[i]) {
            _peak[i] = val;
        } else {
            _peak[i] -= 0.008f; // decay per frame
            if (_peak[i] < 0.0f) _peak[i] = 0.0f;
        }
    }
}

void screen_vumeter_draw(TFT_eSprite& spr) {
    const int cx = 120, cy = 135;
    const int MAX_BAR_LEN = 80;   // max bar length in pixels (radial)
    const int BAR_INNER_R = 28;   // inner radius where bars start
    const float FAN_SPAN   = 180.0f; // total arc span for all bars
    const float FAN_START  = -90.0f + (180.0f - FAN_SPAN) / 2.0f; // centred

    spr.fillSprite(tft.color565(10, 5, 0)); // dark amber background

    // Amber outer ring
    draw_ring_arc(spr, cx, cy, 112, 3, -90.0f, 90.0f, tft.color565(200, 100, 0));

    for (int i = 0; i < NUM_BARS; i++) {
        float a = FAN_START + ((float)i + 0.5f) / NUM_BARS * FAN_SPAN;
        float rad = (a - 90.0f) * DEG_TO_RAD;
        float fx  = cosf(rad);
        float fy  = sinf(rad);

        int bar_len = (int)(_bar[i] * MAX_BAR_LEN);

        // Colour: green → yellow → red by height
        uint16_t col;
        if (_bar[i] < 0.6f) {
            col = tft.color565(0, 200, 0);
        } else if (_bar[i] < 0.85f) {
            col = tft.color565(220, 200, 0);
        } else {
            col = TFT_RED;
        }

        // Draw bar as a radial line from inner_r to inner_r+bar_len
        for (int r = BAR_INNER_R; r < BAR_INNER_R + bar_len; r++) {
            int x = cx + (int)(fx * r);
            int y = cy + (int)(fy * r);
            spr.drawPixel(x, y, col);
            spr.drawPixel(x + 1, y, col); // 2px wide
        }

        // Peak hold dot
        int peak_r = BAR_INNER_R + (int)(_peak[i] * MAX_BAR_LEN);
        if (peak_r > BAR_INNER_R + 2) {
            int px = cx + (int)(fx * peak_r);
            int py = cy + (int)(fy * peak_r);
            spr.fillCircle(px, py, 2, TFT_WHITE);
        }
    }

    // VU label
    spr.setTextDatum(MC_DATUM);
    spr.setTextSize(2);
    spr.setTextColor(tft.color565(255, 140, 0), tft.color565(10, 5, 0));
    spr.drawString("VU", cx, cy - 100);

    // dB scale labels
    spr.setTextSize(1);
    spr.setTextColor(tft.color565(150, 75, 0), tft.color565(10, 5, 0));
    spr.drawString("-20", cx - 60, cy + 25);
    spr.drawString("0",   cx,      cy + 25);
    spr.drawString("+3",  cx + 55, cy + 25);
}
