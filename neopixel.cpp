#include "neopixel.h"

COLOR NEOPIXEL::wheel(uint8_t wheel_pos) {
    wheel_pos = 255 - wheel_pos;
    if(wheel_pos < 85) {
        return Color(255 - wheel_pos * 3, 0, wheel_pos * 3);
    }
    if (wheel_pos < 170) {
        wheel_pos -= 85;
        return Color(0, wheel_pos * 3, 255 - wheel_pos * 3);
    }
    wheel_pos -= 170;
    return Color(wheel_pos * 3, 255 - wheel_pos * 3, 0);
}

COLOR NEOPIXEL::lightWheel(uint8_t wheel_pos) {
    wheel_pos = 255 - wheel_pos;
    if(wheel_pos < 85) {
        return Color(255 - wheel_pos, 255, wheel_pos * 3);
    }
    if (wheel_pos < 170) {
        wheel_pos -= 85;
        return Color(255, wheel_pos * 3, 255 - wheel_pos);
    }
    wheel_pos -= 170;
    return Color(wheel_pos * 3, 255 - wheel_pos, 255);
}

