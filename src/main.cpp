#include <Arduino.h>
#include "display.h"
#include "screens.h"

Screen screens[] = {
    { screen_clock_update,       screen_clock_draw,       "Clock"       },
    { screen_speedometer_update, screen_speedometer_draw, "Speedometer" },
    { screen_rainbow_update,     screen_rainbow_draw,     "Rainbow"     },
    { screen_neon_update,        screen_neon_draw,        "Neon"        },
    { screen_hud_update,         screen_hud_draw,         "HUD"         },
    { screen_vumeter_update,     screen_vumeter_draw,     "VU Meter"    },
};
const int SCREEN_COUNT = 6;

static uint32_t screen_start_ms = 0;
static int      active_screen   = 0;

void setup() {
    display_init();
    screen_start_ms = millis();
    // Seed RNG for HUD data randomness
    randomSeed(esp_random());
}

void loop() {
    uint32_t now = millis();
    if (now - screen_start_ms >= 3000UL) {
        active_screen   = (active_screen + 1) % SCREEN_COUNT;
        screen_start_ms = now;
    }
    float t = (now - screen_start_ms) / 1000.0f;

    screens[active_screen].update(t);
    screens[active_screen].draw(spr);
    spr.pushSprite(0, 0);
}
