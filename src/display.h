#pragma once
#include <TFT_eSPI.h>

extern TFT_eSPI tft;
extern TFT_eSprite spr;

void display_init();

// Convert HSV (each 0.0–1.0) to RGB565
uint16_t hsv_to_rgb565(float h, float s, float v);

// Draw a filled arc ring segment using filled circles along the arc path.
// cx/cy: center, r: radius to center of ring, thickness: ring width in pixels,
// start_deg/end_deg: clockwise from 12-o'clock (0°), color: RGB565
// Note: start_deg must be <= end_deg. Wrapping arcs (e.g. 350°→10°) must be
// split into two calls. Passing start_deg > end_deg draws nothing.
void draw_ring_arc(TFT_eSprite& sprite, int cx, int cy, int r, int thickness,
                   float start_deg, float end_deg, uint16_t color);
