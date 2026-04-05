#include "screens.h"
#include "display.h"
#include <math.h>

static float _speed = 0.0f; // 0–220

void screen_speedometer_update(float t) {
    // Oscillate needle on a 5-second sine cycle
    _speed = 110.0f + 110.0f * sinf(t * (2.0f * (float)M_PI / 5.0f));
}

void screen_speedometer_draw(TFT_eSprite& spr) {
    const int cx = 120, cy = 128;
    spr.fillSprite(TFT_BLACK);

    // Arc spans 135°–405° (270° sweep)
    // Convention: 0° = 12 o'clock, CW
    const float ARC_START = 135.0f;  // 4-o'clock-ish
    const float ARC_END   = 405.0f;  // wraps: 405-360 = 45°

    // Background arc (dark grey)
    draw_ring_arc(spr, cx, cy, 90, 10, ARC_START, ARC_END,
                  tft.color565(40, 40, 40));

    // Coloured filled arc up to needle position
    float fill_end = ARC_START + (_speed / 220.0f) * (ARC_END - ARC_START);
    // Hue: green (0.33) at 0 speed, red (0.0) at max speed
    float hue = 0.33f * (1.0f - _speed / 220.0f);
    uint16_t arc_col = hsv_to_rgb565(hue, 1.0f, 1.0f);
    draw_ring_arc(spr, cx, cy, 90, 10, ARC_START, fill_end, arc_col);

    // Scale labels at 0, 55, 110, 165, 220
    spr.setTextDatum(MC_DATUM);
    spr.setTextSize(1);
    for (int v = 0; v <= 220; v += 55) {
        float a = ARC_START + (v / 220.0f) * (ARC_END - ARC_START);
        float rad = (a - 90.0f) * DEG_TO_RAD;
        int lx = cx + (int)(cosf(rad) * 72);
        int ly = cy + (int)(sinf(rad) * 72);
        spr.setTextColor(TFT_WHITE, TFT_BLACK);
        spr.drawNumber(v, lx, ly);
    }

    // Needle
    float needle_a = ARC_START + (_speed / 220.0f) * (ARC_END - ARC_START);
    float needle_rad = (needle_a - 90.0f) * DEG_TO_RAD;
    int nx = cx + (int)(cosf(needle_rad) * 85);
    int ny = cy + (int)(sinf(needle_rad) * 85);
    spr.drawLine(cx, cy, nx, ny, TFT_WHITE);
    spr.drawLine(cx+1, cy, nx+1, ny, TFT_WHITE); // 2px wide

    // Centre hub
    spr.fillCircle(cx, cy, 10, tft.color565(60, 60, 60));
    spr.fillCircle(cx, cy, 5, TFT_WHITE);

    // Speed readout
    spr.setTextSize(3);
    spr.setTextColor(TFT_WHITE, TFT_BLACK);
    spr.drawNumber((int)_speed, cx, cy + 35);
    spr.setTextSize(1);
    spr.setTextColor(tft.color565(180, 180, 180), TFT_BLACK);
    spr.drawString("km/h", cx, cy + 55);
}
