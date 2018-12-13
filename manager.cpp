#include "manager.h"
#include <stdlib.h>

//---------------------------------------------- Shuffle the animation in the aRandom order --------------------------------
shuffle::shuffle(uint8_t a_size) {
	index = (uint8_t *)malloc(a_size);
	if (!index) a_size = 0;
	for (uint8_t i = 0; i < a_size; ++i) index[i] = i;
	curr = num_anim = a_size;
}

uint8_t shuffle::next(void) {
	if (num_anim == 0) return 0;
	if (curr >= num_anim) randomize();
	return index[curr++];
}

void shuffle::randomize(void) {
	for (uint8_t i = 0; i < num_anim*2; ++i) {
		uint8_t p1 = Random(num_anim);
		uint8_t p2 = Random(num_anim);
    	if (p1 != p2) {
    		uint8_t t	= index[p2];
    		index[p2]	= index[p1];
    		index[p1]	= t;
    	}
	}
	curr = 0;
}

// --------------------------------------------- The sequence manager -----------------------------------------------------
MANAGER::MANAGER(animation* a[], uint8_t a_size, clr* c[], uint8_t clr_size) : shuffle(a_size) {
	anims			= a;
    clearance		= c;
    num_clr			= clr_size;
    stp				= 0;
    do_clear		= false;
    aIndex			= 0;
}

void MANAGER::init(void) {
	if (!anims[aIndex]->do_clear) {
		aIndex = shuffle::next();
//		aIndex = 44;
		a = anims[aIndex];

		uint32_t period = a->show_time;
		period = Random(period, period * 3);						// time in 10-seconds intervals
		period *= 10000;											// Now time in ms
		next    = millis() + period;
	}

	a = anims[aIndex];
	strip.clear();
	a->init();														// Initialization procedure can change period parameters (min_p & max_p)
	uint16_t min_stp = uint16_t(a->min_p) * 10;
	uint16_t max_stp = uint16_t(a->max_p) * 10;
	stp_period = Random(min_stp, max_stp) + 1;						// If a->min_p == 0, ensure the step period is greater than 0
	stp = 0;

	strip.show();
	a->do_clear = false;
}

void MANAGER::show(void) {
	uint32_t ms = millis();
	if (!do_clear && (ms > next) && a->complete) {					// The current animation is timed out
		if (isClean())
			init();
		else
			initClear();
	}

	if (ms < stp) {													// It is not time for next step, return
		return;
	}

	if (do_clear)
		stp = ms + clr_stp_period;
	else
		stp = ms + stp_period;

	if (do_clear) {
		if (c->isComplete()) {
			do_clear = false;
			if (ms > next) a->do_clear = false;						// It is too late to continue the animation
			init();
		} else
			c->show();												// Keep running clear session till it ends
	} else {
		if (a->do_clear) initClear();
		a->show();
	}
	strip.show();
}


void MANAGER::initClear(void) {
	do_clear = true;												// Start clearing sequence
	uint8_t ni = Random(num_clr);
	c = clearance[ni];
	c->init();
	stp = 0;
	clr_stp_period = Random(3, 10) * 10;
}

bool MANAGER::isClean(void) {
	for (uint16_t i = 0; i < strip.numPixels(); ++i)
		if (strip.getPixelColor(i)) return false;
	return true;
}
