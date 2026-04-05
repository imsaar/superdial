#include "screens.h"
#include "display.h"
#include <math.h>

// Draws a clock hand from center (cx,cy) outward at angle_deg (0=12 o'clock, CW),
// length pixels long, width pixels wide, in the given color.
static void draw_hand(TFT_eSprite& spr, int cx, int cy, float angle_deg,
                      int length, int width, uint16_t color) {
    // Rotate: 0°=top means subtract 90° from standard math angle
    float rad = (angle_deg - 90.0f) * DEG_TO_RAD;
    int x2 = cx + (int)(cosf(rad) * length);
    int y2 = cy + (int)(sinf(rad) * length);
    // Draw multiple offset lines for width
    for (int w = -width/2; w <= width/2; w++) {
        float perp = rad + (float)M_PI / 2.0f;
        int ox = (int)(cosf(perp) * w);
        int oy = (int)(sinf(perp) * w);
        spr.drawLine(cx + ox, cy + oy, x2 + ox, y2 + oy, color);
    }
}

void screen_clock_update(float t) {
    // No per-frame state needed — draws directly from millis()
}

void screen_clock_draw(TFT_eSprite& spr) {
    const int cx = 120, cy = 120, face_r = 108;
    spr.fillSprite(TFT_BLACK);

    // Gradient-ish face: filled dark-blue circle
    spr.fillCircle(cx, cy, face_r, tft.color565(10, 10, 40));

    // Outer bezel ring
    draw_ring_arc(spr, cx, cy, face_r, 4, -90.0f, 270.0f, tft.color565(80, 80, 200));

    // Tick marks
    for (int i = 0; i < 60; i++) {
        float a = i * 6.0f - 90.0f; // degrees from 12 o'clock
        float rad = a * DEG_TO_RAD;
        bool major = (i % 5 == 0);
        int inner_r = major ? face_r - 14 : face_r - 7;
        int outer_r = face_r - 3;
        uint16_t col = major ? TFT_WHITE : tft.color565(80, 80, 120);
        int x1 = cx + (int)(cosf(rad) * inner_r);
        int y1 = cy + (int)(sinf(rad) * inner_r);
        int x2 = cx + (int)(cosf(rad) * outer_r);
        int y2 = cy + (int)(sinf(rad) * outer_r);
        spr.drawLine(x1, y1, x2, y2, col);
        if (major) spr.drawLine(x1+1, y1, x2+1, y2, col); // thicker for major
    }

    // Time from millis() — simulated clock starting at boot
    uint32_t ms = millis();
    float sec_angle  = (ms % 60000UL)       / 60000.0f       * 360.0f;
    float min_angle  = (ms % 3600000UL)     / 3600000.0f     * 360.0f;
    float hour_angle = (ms % 43200000UL)    / 43200000.0f    * 360.0f;

    // Hour hand — white, thick, short
    draw_hand(spr, cx, cy, hour_angle, 55, 5, TFT_WHITE);
    // Minute hand — cyan, medium
    draw_hand(spr, cx, cy, min_angle, 78, 3, tft.color565(0, 220, 255));
    // Second hand — red, thin with counterweight tail
    draw_hand(spr, cx, cy, sec_angle, 90, 1, TFT_RED);
    draw_hand(spr, cx, cy, sec_angle + 180.0f, 20, 1, TFT_RED); // counterweight

    // Center cap
    spr.fillCircle(cx, cy, 7, TFT_RED);
    spr.fillCircle(cx, cy, 3, TFT_WHITE);
}
