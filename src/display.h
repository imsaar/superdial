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
// Note: start_deg must be <= end_deg. end_deg may exceed 360 (e.g. 300°→500°
// draws a 200° arc, crossing the 360°/0° boundary correctly via sinf/cosf).
// Backward arcs (start_deg > end_deg) draw nothing.
void draw_ring_arc(TFT_eSprite& sprite, int cx, int cy, int r, int thickness,
                   float start_deg, float end_deg, uint16_t color);
