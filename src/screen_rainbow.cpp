#include "screens.h"
#include "display.h"
#include <math.h>

static float _marker_angle = 0.0f;
static float _breathe = 0.0f;

void screen_rainbow_update(float t) {
    _marker_angle = fmodf(t * 72.0f, 360.0f); // one full sweep every 5 seconds
    _breathe = 0.5f + 0.5f * sinf(t * 2.0f);  // 0..1 brightness pulse
}

void screen_rainbow_draw(TFT_eSprite& spr) {
    const int cx = 120, cy = 120;

    // Breathing background
    uint8_t bg = (uint8_t)(_breathe * 20);
    spr.fillSprite(tft.color565(0, 0, bg));

    // Full-spectrum arc: draw in small steps, each step a different hue
    const float STEP = 1.2f;
    for (float a = 0.0f; a < 360.0f; a += STEP) {
        float hue = a / 360.0f;
        uint16_t col = hsv_to_rgb565(hue, 1.0f, 1.0f);
        float rad = (a - 90.0f) * DEG_TO_RAD;
        // Outer arc at r=100, inner at r=82 (18px thick)
        for (int r = 82; r <= 100; r++) {
            int x = cx + (int)(cosf(rad) * r);
            int y = cy + (int)(sinf(rad) * r);
            spr.drawPixel(x, y, col);
        }
    }

    // Inner gold outline ring
    draw_ring_arc(spr, cx, cy, 76, 3, -90.0f, 270.0f, tft.color565(255, 200, 0));

    // Marker: bright white spike at current position
    float mrad = (_marker_angle - 90.0f) * DEG_TO_RAD;
    for (int r = 78; r <= 104; r++) {
        int x = cx + (int)(cosf(mrad) * r);
        int y = cy + (int)(sinf(mrad) * r);
        spr.fillCircle(x, y, 2, TFT_WHITE);
    }

    // Centre: hue label (current hue percentage)
    spr.setTextDatum(MC_DATUM);
    spr.setTextSize(2);
    spr.setTextColor(TFT_WHITE, tft.color565(0, 0, bg));
    char buf[8];
    snprintf(buf, sizeof(buf), "%d%%", (int)(_marker_angle / 3.6f));
    spr.drawString(buf, cx, cy - 10);

    spr.setTextSize(1);
    spr.setTextColor(tft.color565(200, 200, 200), tft.color565(0, 0, bg));
    spr.drawString("HUE", cx, cy + 12);
}
