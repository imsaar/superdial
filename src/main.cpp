#include <Arduino.h>
#include "display.h"
#include "screens.h"

void setup() {
    display_init();
}

void loop() {
    spr.fillSprite(TFT_BLACK);
    spr.fillCircle(120, 120, 50, TFT_RED); // sanity-check circle
    spr.pushSprite(0, 0);
    delay(16);
}
