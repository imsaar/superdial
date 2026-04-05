#include "display.h"

TFT_eSPI tft;
TFT_eSprite spr(&tft);

void display_init() {
    tft.init();
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);

    // 16-bit colour sprite — 240×240×2 = ~112 KB, fits in ESP32-C3's 400 KB SRAM
    spr.setColorDepth(16);
    spr.createSprite(240, 240);
    spr.fillSprite(TFT_BLACK);
}

uint16_t hsv_to_rgb565(float h, float s, float v) {
    float r, g, b;
    int i = (int)(h * 6.0f);
    float f = h * 6.0f - i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - f * s);
    float t = v * (1.0f - (1.0f - f) * s);
    switch (i % 6) {
        case 0: r=v; g=t; b=p; break;
        case 1: r=q; g=v; b=p; break;
        case 2: r=p; g=v; b=t; break;
        case 3: r=p; g=q; b=v; break;
        case 4: r=t; g=p; b=v; break;
        case 5: r=v; g=p; b=q; break;
        default: r=0; g=0; b=0; break;
    }
    return tft.color565((uint8_t)(r * 255), (uint8_t)(g * 255), (uint8_t)(b * 255));
}

void draw_ring_arc(TFT_eSprite& spr, int cx, int cy, int r, int thickness,
                   float start_deg, float end_deg, uint16_t color) {
    float step = 0.8f; // degrees per dot — smoother than 1° for large radii
    int dot_r = max(1, thickness / 2);
    for (float a = start_deg; a <= end_deg; a += step) {
        // Convert from clockwise-from-12-o'clock to math angle:
        // 0° (12 o'clock) → -90° math, increasing clockwise → subtract 90°
        float rad = (a - 90.0f) * DEG_TO_RAD;
        int x = cx + (int)(cosf(rad) * r);
        int y = cy + (int)(sinf(rad) * r);
        spr.fillCircle(x, y, dot_r, color);
    }
}
