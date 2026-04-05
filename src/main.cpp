#include <Arduino.h>
#include "display.h"
#include "screens.h"

Screen screens[] = {
    { screen_clock_update,       screen_clock_draw,       "Clock"       },
    { screen_speedometer_update, screen_speedometer_draw, "Speedometer" },
};
const int SCREEN_COUNT = 2;

static uint32_t screen_start_ms = 0;
static int active_screen = 0;

void setup() {
    display_init();
    screen_start_ms = millis();
}

void loop() {
    uint32_t now = millis();
    if (now - screen_start_ms >= 3000UL) {
        active_screen = (active_screen + 1) % SCREEN_COUNT;
        screen_start_ms = now;
    }
    float t = (now - screen_start_ms) / 1000.0f;
    screens[active_screen].update(t);
    spr.fillSprite(TFT_BLACK);
    screens[active_screen].draw(spr);
    spr.pushSprite(0, 0);
}
