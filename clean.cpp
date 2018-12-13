#include "clean.h"

//---------------------------------------------- Classes for strip clearing  ----------------------------------------------
bool clr::fadeAll(uint8_t val) {
	bool finish = true;
	for (uint16_t i = 0; i < strip.numPixels(); ++i) {
		if (!fade(i, val)) finish = false;
	}
	return finish;
}

bool clr::fade(uint16_t index, uint8_t val) {
	COLOR c = strip.getPixelColor(index);
	uint8_t bound = 0;
	for (int8_t s = 16; s >= 0; s -= 8) {
		int32_t cc = c >> s;										// The color component (red, green or blue)
		cc &= 0xff;
		cc -= int(val);
		if (cc < 0) {
			cc = 0;
			bound ++;
		}
		uint32_t mask = 0xff; mask <<= s; mask = ~mask;
		cc <<= s;
		c &= mask;
		c |= cc;
	}
	strip.setPixelColor(index, c);
	return (bound >= 3);
}

bool clr::isComplete(void) {
	return complete;
}

// --------------------------------------------- Clear the strip from the ether side --------------------------------------
void clearSide::init(void) {
	complete = false;
	color    = strip.wheel(Random(256));
	fwd      = Random(2);
	if (fwd)
		index = 0;
	else
		index = strip.numPixels() - 1;
}

void clearSide::show(void) {
	if (fwd) {
		if (index < int(strip.numPixels())) {
			strip.setPixelColor(index, color);
			if (index > 0) strip.setPixelColor(index-1, 0);
		}
		++index;
		complete = (index >= int(strip.numPixels()));
	} else {
		if (index >= 0) {
			strip.setPixelColor(index, color);
			if (index < int(strip.numPixels() - 1)) strip.setPixelColor(index+1, 0);
		}
		--index;
		complete = (index < 0);
	}
}

// --------------------------------------------- Clear the strip from the center to both ends -----------------------------
void clearCntr::init(void) {
	complete = false;
	color = strip.wheel(Random(256));
	l = strip.numPixels() / 2;
	r = l + 1;
}

void clearCntr::show(void) {
	if (r < int(strip.numPixels())) {
		strip.setPixelColor(r, color);
		if (r > 0) strip.setPixelColor(r-1, 0);
	}
	++r;
	if (l >= 0) {
		strip.setPixelColor(l, color);
		if (l < int(strip.numPixels() - 1)) strip.setPixelColor(l+1, 0);
	}
	--l;
	complete = (l < 0);
}

// --------------------------------------------- Clear the strip by 'eating' the pixels from the center -------------------
void eatCntr::show(void) {
	int n = strip.numPixels();
	int c = n / 2;
	for (int i = c; i > 0; --i) {
		COLOR c = strip.getPixelColor(i-1);
		strip.setPixelColor(i, c);
	}
	for (int i = c; i < n-1; ++i) {
		COLOR c = strip.getPixelColor(i+1);
		strip.setPixelColor(i, c);
	}
	strip.setPixelColor(0, 0);
	strip.setPixelColor(n-1, 0);
	--remain;
	complete = (remain <= 0);
}

//---------------------------------------------- Clear the strip by deviding it by 2 ------------------------------------
void clearHalf::init(void) {
	complete = false;
	one_step = strip.numPixels() / 2;
}

void clearHalf::show(void) {
	for (uint16_t i = 0; i < strip.numPixels(); i += one_step) {
		if (i > 0 || (one_step == 1)) strip.setPixelColor(i, 0);
	}
	complete = ((one_step >>= 1) == 0);
}


