#include "clrutils.h"

// --------------------------------------------- creep the sequence up or down, superclass --------------------------------
void CRAWL::step(void) {
	if (fwd) {												// creep forward
		for (int i = strip.numPixels()-1; i > 0; --i) {
			uint32_t c = strip.getPixelColor(i-1);
			strip.setPixelColor(i, c);
		}
		strip.setPixelColor(0, next_color);
	} else {												// creep backward
		int last = strip.numPixels()-1;
		for (int i = 0; i < last; ++i) {
			uint32_t c = strip.getPixelColor(i+1);
			strip.setPixelColor(i, c);
		}
		strip.setPixelColor(last, next_color);
	}
}

//---------------------------------------------- Brightness manipulation --------------------------------------------------
void BRGTN::setColor(COLOR c) {
	color[0] = c & 0xff;	c >>= 8;									// Blue
	color[1] = c & 0xff;	c >>= 8;									// Green
	color[2] = c & 0xff;												// Red
}

bool BRGTN::changeClr(COLOR& c, int8_t val) {
	COLOR	new_color	= 0;
	uint8_t	bound		= 0;

	if (val < 0) {														// Decrement color
		if (c == 0)														// Cannot decrement zero color
			return true;
		int16_t  e = 256 + val;
		for (uint8_t i = 0; i < 3; ++i) {
			int32_t cc = c & 0xff; c >>= 8;								// Color component value (b, g, r)
			int32_t new_cc = cc * e; new_cc >>= 8;						// New color component = (cc * e) / 256;
			if (new_cc == cc) {											// For small COLOR component value use addition instead of multiplication
				cc += val;
			} else {
				cc = new_cc;
			}
			if (cc <= 0) {
				cc = 0;
				++bound;
			}
			new_color >>= 8;
			cc <<= 16; new_color |= cc;									// Build new color by shift the components from left to right
		}
	} else {															// Increment color
		for (uint8_t i = 0; i < 3; ++i) {
			int16_t e = (color[i] * val) >> 8;
			if (e == 0) e = 1;
			int32_t cc = c & 0xff; c >>= 8;								// Color component value (b, g, r)
			cc += e;
			if (cc >= color[i]) {										// Limit component value by the component of the preset color
				cc = color[i];
				++bound;
			}
			new_color >>= 8;
			cc <<= 16; new_color |= cc;									// Build new color by shift the components from left to right
		}
	}

	c = new_color;
	return (bound >= 3);
}

bool BRGTN::change(uint16_t index, int8_t val) {
	COLOR c = strip.getPixelColor(index);
	bool done = changeClr(c, val);
	strip.setPixelColor(index, c);
	return done;
}

bool BRGTN::changeAll(int8_t val) {
	bool finish = true;
	for (uint16_t i = 0; i < strip.numPixels(); ++i) {
		COLOR c = strip.getPixelColor(i);
		if (!changeClr(c, val)) finish = false;
		strip.setPixelColor(i, c);
	}
	return finish;
}

//---------------------------------------------- Blend manipulations ------------------------------------------------------
COLOR BLEND::add(COLOR color1, COLOR color2) {
	uint8_t r1,g1,b1;
	uint8_t r2,g2,b2;

	r1 = (uint8_t)(color1 >> 16);
	g1 = (uint8_t)(color1 >>  8);
	b1 = (uint8_t)(color1 >>  0);

	r2 = (uint8_t)(color2 >> 16);
	g2 = (uint8_t)(color2 >>  8);
	b2 = (uint8_t)(color2 >>  0);

	return strip.Color(constrain(r1+r2, 0, 255), constrain(g1+g2, 0, 255), constrain(b1+b2, 0, 255));
}

COLOR BLEND::sub(COLOR color1, COLOR color2) {
	uint8_t r1,g1,b1;
	uint8_t r2,g2,b2;
	int16_t r,g,b;

	r1 = (uint8_t)(color1 >> 16);
	g1 = (uint8_t)(color1 >>  8);
	b1 = (uint8_t)(color1 >>  0);

	r2 = (uint8_t)(color2 >> 16);
	g2 = (uint8_t)(color2 >>  8);
	b2 = (uint8_t)(color2 >>  0);

	r  = (int16_t)r1 - (int16_t)r2;
	g  = (int16_t)g1 - (int16_t)g2;
	b =  (int16_t)b1 - (int16_t)b2;
	if (r < 0) r = 0;
	if (g < 0) g = 0;
	if (b < 0) b = 0;

	return strip.Color(r, g, b);
}

void BLEND::blendPixel(uint16_t p, uint8_t deviation) {
	if (deviation < 3) deviation = 3;
	uint8_t r1	= Random(deviation);
	uint8_t g1	= Random(deviation);
	uint8_t b1	= Random(deviation);
	uint32_t 	diff_color 		= strip.Color(r1, g1, b1);
	uint32_t 	blended_color 	= add(diff_color, strip.getPixelColor(p));
	r1	= Random(deviation);
	g1 	= Random(deviation);
	b1 	= Random(deviation);
	diff_color		= strip.Color(r1, g1, b1);
	blended_color 	= sub(blended_color, diff_color);
	strip.setPixelColor(p, blended_color);
}

//---------------------------------------------- Color superposition class ------------------------------------------------
COLOR TWOCLR::add(COLOR c1, COLOR c2) {
	COLOR c = 0;
	for (uint8_t i = 0; i < 3; ++i) {
		uint32_t p = c1 >> (i*8);
		p += c2 >> (i*8);
		p &= 0xff;
		p <<= i*8;
		c |= p;
	}
	return c;
}

//---------------------------------------------- The sine wave value class ------------------------------------------------
uint8_t	WAVE::amplitude(uint8_t n) {
	n &= 0x3F;															// The sin(PI*n/32) period is 64
	if (n >= 32) {
		n -= 32;
		if (n > 16)
			return 254 - sine[32-n];
		else
			return 254 - sine[n];
	} else {
		if (n > 16)
			return sine[32-n];
		else
			return sine[n];
	}
}

COLOR	WAVE::intencity(COLOR c, uint8_t n) {
	COLOR res = 0;
	for (uint8_t i = 0; i < 3; ++i) {									// Get the color components: b, g, r
		res >>= 8;
		uint32_t cc = c & 0xFF;	c >>= 8;
		cc *= amplitude(n);
		cc /= 254;
		cc <<= 16; res |= cc;
	}
	return res;
}
