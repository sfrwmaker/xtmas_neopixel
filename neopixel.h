#ifndef __NEOPIXEL_H
#define __NEOPIXEL_H
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

typedef uint32_t COLOR;

class NEOPIXEL : public Adafruit_NeoPixel {
    public:
        NEOPIXEL(uint16_t n, uint8_t p, uint16_t t) : Adafruit_NeoPixel(n, p, t)    { }
        COLOR   wheel(uint8_t wheel_pos);
        COLOR   lightWheel(uint8_t wheel_pos);
};

#endif
