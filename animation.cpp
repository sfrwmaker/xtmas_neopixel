#include "animation.h"
#include "math.h"

//---------------------------------------------- Base animation class with useful functions -------------------------------
animation::animation(void) {
	min_p		= 2;
	max_p		= 8;
	show_time	= min_time / 10;
	do_clear 	= false;
	complete 	= true;
}

// --------------------------------------------- Fill the dots one after the other with a color ---------------------------
void colorWipe::init(void) {
	int p 	= Random(2, 4);
	w  	   += p*16 + 1;
	fwd 	= Random(2);
	index	= 0;
	if (!fwd) index = strip.numPixels() - 1;
}

void colorWipe::show(void) {
	uint32_t color = strip.wheel(w);
	if (fwd) {
		if (index > int(strip.numPixels())) {						// Start new sequence with the new color
			init();
			complete = true;
			return;
		}
		strip.setPixelColor(index++, color);
	} else {
		if (index < 0) {											// Start new sequence with the new color
			init();
			complete = true;
			return;
		}
		strip.setPixelColor(index--, color);
	}
	complete = false;
}

// --------------------------------------------- Walk the dots one after the other with a color ---------------------------
void colorWalk::init(void) {
	index	= 0;
	w		= Random(256);
	fwd    	= Random(2);
	period 	= Random(10, 30);
}

void colorWalk::show(void) {
	int n = strip.numPixels();
	if (fwd) {
		if (index > n) {
			index -= period;
			strip.setPixelColor(n-1, 0);
		}
		uint32_t color = strip.wheel(w--);
		for (int i = index; i > 0; i -= period) {
			if (i > 0) strip.setPixelColor(i-1, 0);
			strip.setPixelColor(i, color);
		}
		++index;
	} else {
		if (index < 0) {
			index += period;
			strip.setPixelColor(0, 0);
		}
		uint32_t color = strip.wheel(w++);
		for (int i = index; i < n; i += period) {
			if (i < int(strip.numPixels() - 1)) strip.setPixelColor(i+1, 0);
			strip.setPixelColor(i, color);
		}
		--index;
	}
}

// --------------------------------------------- creep the aRandom sequence up or down -------------------------------------
void randomCreep::init(void) {
	space 				= Random(2, 5);
	change_direction 	= Random(100, 500);
	cnt 				= 0;
}

void randomCreep::show(void) {
	-- change_direction;
	if (change_direction <= 0) {
		fwd = !fwd;
		init();
	}

	next_color = 0;
	++cnt;
	if (cnt > space) {
		cnt = 0;
		next_color = strip.wheel(Random(256));
	}
	step();
}

// --------------------------------------------- show the color Wave using rainbowCycle -----------------------------------
void colorWave::init(void) {
	index = 0;
	rdy = false;
	fwd = Random(2);
}

void colorWave::show(void) {
	if (!rdy) {
		rdy = true;
		for (uint16_t i = 0; i < strip.numPixels(); ++i) {
			setColor(strip.wheel(((i * 256 / strip.numPixels())) & 255));
			if (!change(i, 2)) rdy = false;
		}
		return;
	}

	step();
	if (fwd)
		strip.setPixelColor(0, strip.wheel(index & 255));
	else
		strip.setPixelColor(strip.numPixels() - 1, strip.wheel(index & 255));
	++index;
}

// --------------------------------------------- show the rainbow (from the NEOPIXEL example) -----------------------------
void rainbow::show(void) {
	if (!rdy) {
		rdy = true;
		for (uint16_t i = 0; i < strip.numPixels(); ++i) {
			setColor(strip.wheel(i & 255));
			if (!change(i, 2)) rdy = false;
		}
		return;
	}

	for(uint16_t i = 0; i < strip.numPixels(); ++i) {
		strip.setPixelColor(i, strip.wheel((i+index) & 255));
	}
	++index;													// index is from 0 to 255
}

// --------------------------------------------- show the rainbowCycle (from the NEOPIXEL example) ------------------------
void rainCycle::show(void) {
	if (!rdy) {
		rdy = true;
		for (uint16_t i = 0; i < strip.numPixels(); ++i) {
			setColor(strip.wheel((i * 256 / strip.numPixels()) & 255));
			if (change(i, 1)) rdy = false;
		}
		return;
	}

	for(uint16_t i = 0; i < strip.numPixels(); ++i) {
		strip.setPixelColor(i, strip.wheel(((i * 256 / strip.numPixels()) + index) & 255));
	}
	++index;													// index is from 0 to 255
}

// --------------------------------------------- show the rainbow colors on the entire strip ------------------------------
void rainFull::show(void) {
	if (!rdy) {
		rdy = true;
		for (uint16_t i = 0; i < strip.numPixels(); ++i) {
			setColor(strip.wheel(index));
			if (!change(i, 1)) rdy = false;
		}
		return;
	}

	for (uint16_t i = 0; i < strip.numPixels(); ++i) {
		strip.setPixelColor(i, strip.wheel(index));
	}
	++index;													// index is from 0 to 255
}

//---------------------------------------------- Light up with the aRandom color than fade out -----------------------------
void lightUp::show(void) {
	bool finish = changeAll(inc);
	if (finish) {
		if (inc > 0) {
			inc = -8;
		} else {
			++sp;
			inc = sp;
			if (sp > 8) {
				sp = 1;
				newColor();
			}
		}
	}
}

void lightUp::newColor(void) {
	uint32_t c = 0;
	for (uint8_t i = 0; i < 3; ++i) {
		c <<= 8;
		uint8_t d = Random(16) << 4;
		c |= d;
	}
	setColor(c);
}

//---------------------------------------------- Random sparcs ------------------------------------------------------------
void sparks::show(void) {
	uint32_t c = strip.wheel(Random(256));
	for (int8_t i = 7; i >= 1; --i) {
		if (i == 6)
			strip.setPixelColor(pos[uint8_t(i)], 0);
		else
			change(pos[uint8_t(i)], -128);
		pos[uint8_t(i)] = pos[uint8_t(i-1)];
	}
	int16_t p = Random(strip.numPixels()+1);
	pos[0] = p;
	strip.setPixelColor(p, c);
}

//---------------------------------------------- Random sparks fade out ---------------------------------------------------
void rndFade::show(void) {
	changeAll(-16);
	uint8_t newDot = Random(1, 5);
	for (uint8_t i = 0; i < newDot; ++i) {
		uint16_t p		= Random(strip.numPixels()+1);
		uint32_t c	= strip.wheel(Random(256));
		if (strip.getPixelColor(p) == 0)
			strip.setPixelColor(p, c);
	}
}

//---------------------------------------------- Lights run from the center -----------------------------------------------
void centerRun::init(void) {
	color		= strip.wheel(Random(256));
	int n		= strip.numPixels();
	int diff	= n >> 3;
	m = l = r = Random(diff+2) + ((n * 7) >> 4);
}

void centerRun::show(void) {
	if (l >= 0) strip.setPixelColor(l, color);
	if (r < int(strip.numPixels())) strip.setPixelColor(r, color);
	l -= 2; r += 2;
	if ((l < 0) && r >= int(strip.numPixels())) {
		l = r = m + 1;
		color = strip.wheel(Random(256));
	}
}

//---------------------------------------------- Slow shining by the different colors -------------------------------------
void shineSeven::init(void) {
	curs = 0;
	w    = Random(256);
	base = Random(4, 8);
	startNewColor();
}

void shineSeven::show() {
	int16_t n = strip.numPixels();
	bool finish = true;
	for (int16_t i = int16_t(curs) - 1; i < n; i += base) {		// Fade out previous color
		if (i >= 0)
			if (!change(i, -8)) finish = false;
	}
	for (int16_t i = curs; i < n; i += base)					// Light up current color
    if (!change(i, 8)) finish = false;
	if (finish) {												// The current color has been light fully
		++curs; if (curs >= base) curs = 0;
		startNewColor();
	}
}

void shineSeven::startNewColor(void) {
	uint32_t c = strip.wheel(w);
	w += 97;
	setColor(c);
	c &= 0x10101;
	for (uint16_t i = curs; i < strip.numPixels(); i += base)
		strip.setPixelColor(i, c);
}

//---------------------------------------------- Rapid walking by the different colors -----------------------------------
void walkSeven::init() {
	w		= Random(256);
	fwd		= Random(2);
	ch_dir	= Random(30, 100);
	period	= Random(13, 20);
	curs	= 0;
}

void walkSeven::show(void) {
	changeAll(-64);

	int n = strip.numPixels();
	uint32_t c1 = strip.wheel(w);
	w += 71;
	for (int i = curs; i < n; i += period) {
		uint32_t c2 = strip.getPixelColor(i);
		c2 = TWOCLR::add(c1, c2);
		strip.setPixelColor(i, c2);
	}

	if (fwd) ++curs; else --curs;
	curs %= period;

	if (--ch_dir < 0) {
		ch_dir = Random(70, 300);
		fwd = !fwd;
	}
}

//---------------------------------------------- Rapid flashing by the differenc colors ---------------------------------
void flashSeven::init() {
	w		= Random(256);
	fwd		= Random(2);
	ch_dir	= Random(30, 100);
	period	= Random(7, 20);
	curs	= 0;
}

void flashSeven::show(void) {
	changeAll(-64);

	int n = strip.numPixels();
	for (int i = curs; i < n; i += period) {
		change(i, -128);
	}

	if (fwd) ++curs; else --curs;
	curs %= period;

	uint32_t c = strip.wheel(w);
	w += 71;
	for (int i = curs; i < n; i += period) {
		strip.setPixelColor(i, c);
	}

	if (--ch_dir < 0) {
		ch_dir = Random(70, 300);
		fwd = !fwd;
	}
}

//---------------------------------------------- Slow merging of two colors -----------------------------------------------
void mergeOne::init(void) {
	l 	= 0;
	r 	= strip.numPixels();
	uint8_t indx = Random(256);
	cl 	= strip.wheel(indx);
	indx += Random(4, 16);
	cr 	= strip.wheel(indx);
	strip.clear();
}

void mergeOne::show(void) {
	if (l < r) {
		strip.setPixelColor(l, cl);
		strip.setPixelColor(r, cr);
	} else {
		uint32_t c = strip.getPixelColor(l);
		c = add(c, cl);
		strip.setPixelColor(l, c);
		c = strip.getPixelColor(r);
		c = add(c, cr);
		strip.setPixelColor(r, c);
	}

	--r; ++l;
	if (r < 0) {													// Force the strip to be clean
		do_clear = true;
		complete = true;
		return;
	}
	complete = false;
}

//---------------------------------------------- Fast merging of Waves ----------------------------------------------------
void mergeWave::init(void) {
	l		= 0;
	r		= strip.numPixels() - 1;
	index	= Random(256);
	len		= Random(8, 17);
	strip.clear();
}

void mergeWave::show(void) {
	if (l < r) {
		strip.setPixelColor(l, strip.wheel(l & 255));
		if (l > len) strip.setPixelColor(l-len, 0);
		strip.setPixelColor(r, strip.wheel((index + r) & 255));
		if ((r + len) > int(strip.numPixels())) strip.setPixelColor(r+len, 0);
	} else {
		uint32_t c = strip.getPixelColor(l);
		c |= strip.wheel(l & 255);
		strip.setPixelColor(l, c);
		c = strip.getPixelColor(r);
		c |= strip.wheel((index + r) & 255);
		strip.setPixelColor(r, c);
		strip.setPixelColor(l-len, 0);
		strip.setPixelColor(r+len, 0);
	}
	--r; ++l; index += 4;
	if (r < 0) {														// Force the strip to be clean
		do_clear = true;
		complete = true;
	}
	complete = false;
}

//---------------------------------------------- Fast collide of two colors ---------------------------------------------
void collideOne::init(void) {
	l = 0;
	r = strip.numPixels() - 1;
	uint8_t w = Random(256);
	cl = strip.wheel(w);
	w += Random(8, 16);
	cr = strip.wheel(w);
	strip.clear();
	boom = false;
}

void collideOne::show(void) {
	if (boom) {
		strip.setPixelColor(l, 0xffffff);
		strip.setPixelColor(r, 0xffffff);
		if ((r - l) >= 12) {
			strip.setPixelColor(l+6, 0);
			strip.setPixelColor(r-6, 0);
		}
		--l; ++r;
		if (l < 0) {
			init();
			complete = true;
			return;
		}
	} else {
		if (l < r) {
			strip.setPixelColor(l, cl);
			strip.setPixelColor(r, cr);
			--r; ++l;
		} else {
			boom = true;
		}
	}
	complete = false;
}

//---------------------------------------------- Neo fire animation by Robert Ulbricht ----------------------------------
void neoFire::init(void) {
	color = 0;
	uint8_t m = Random(3);
	color |= c1 << (m*8);
	m += Random(1,3);
	if (m >= 3) m = 0;
	color |= c2 << (m*8);
}

void neoFire::show(void) {
	if (pause > 0) {
		--pause;
		return;
	}
	pause = Random(8);
	for (uint16_t i = 0; i < strip.numPixels(); ++i) {
		uint32_t blended_color = add(strip.getPixelColor(i), color);
		uint8_t r = Random(80);
		uint32_t diff_color = strip.Color(r, r/2, r/2);
		blended_color = sub(blended_color, diff_color);
		strip.setPixelColor(i, blended_color);
	}
}

//---------------------------------------------- Even and odd position leds are moving towards each other ---------------
void evenOdd::init(void) {
	uint8_t indx = Random(256);
	cl = strip.wheel(indx);
	indx += Random(4, 16);
	cr = strip.wheel(indx);
	l = 0;
	r = strip.numPixels() - 1;											// r is Odd
}

void evenOdd::show(void) {
	strip.setPixelColor(l, cl);
	strip.setPixelColor(r, cr);
	l += 2; r -= 2;
	if (r < 0) {
		if (cr) {
			cr = cl = 0;
			l = 0;
			r = strip.numPixels() - 1;									// r is Odd
		} else {
			init();
			complete = true;
			return;
		}
	}
	complete = false;
}

//---------------------------------------------- aRandom colors from left and right move to the center -------------------
void collMdl::init(void) {
	ml = (strip.numPixels() - 1) / 2;
	mr = ml + 1;
	newColors();
	clr = false;
}

void collMdl::show(void) {
	if (clr) {
		strip.setPixelColor(l, 0);
		strip.setPixelColor(r, 0);
		l--; r++;
		if (l < 0) {
			init();
			complete = true;
		}
		return;
	}

	// blend colors in the middle
	if ((mr - ml) > 1) {
		for (int i = ml; i < mr; ++i)
			blendPixel(i);
	}

	// New colors are moving to the center
	if (l <= ml) {
		if (l > 1)strip.setPixelColor(l-2, 0);
		strip.setPixelColor(l, cl);
	}
	if (r >= mr) {
		if (r < int(strip.numPixels() - 2)) strip.setPixelColor(r+2, 0);
		strip.setPixelColor(r, cr);
	}
	if ((l >= ml) && (r <= mr)) {
		ml--; mr++;
		if (ml < 0) {
			clr = true;
			l = (strip.numPixels() - 1) / 2;
			r = l + 1;
			return;
		}
		newColors();
		return;
	}
	l++; r--;
	complete = false;
}

void collMdl::newColors(void) {
	cl = strip.wheel(Random(256));
	cr = strip.wheel(Random(256));
	l = 0;
	r = strip.numPixels() - 1;
}

//------------------------------------------- aRandom colors from left and right move to the other end -------------------
void collEnd::init(void) {
	ml = strip.numPixels() - 1;
	mr = 0;
	newColors();
}

void collEnd::show(void) {
	// blend colors in the both ends
	if (mr > 1) {
		for (int i = 0; i < mr; ++i)
			blendPixel(i);
		for (uint16_t i = ml; i < strip.numPixels(); ++i)
			blendPixel(i);
	}

	// New colors are moving to the other end
	if (l <= ml) {
		if (l > (mr + 1))
			strip.setPixelColor(l-2, 0);
		strip.setPixelColor(l, cl);
	}
	if (r >= mr) {
		if (r < (ml - 1))
			strip.setPixelColor(r+2, 0);
		strip.setPixelColor(r, cr);
	}
	if ((l >= ml) && (r <= mr)) {
		ml--; mr++;
		if (ml < 0) {
			do_clear = true;                                    // Force the strip to be cleaned
			complete = true;
			return;
		}
		newColors();
		return;
	}
	l++; r--;
	complete = false;
}

void collEnd::newColors(void) {
	cl = strip.wheel(Random(256));
	cr = strip.wheel(Random(256));
	l = mr;
	r = ml;
}

//------------------------------------------- Rainbow colors blend --------------------------------------------------------
void rainBlend::show(void) {
	if (index < int(strip.numPixels())) {
		strip.setPixelColor(index, strip.wheel(((index * 256 / strip.numPixels())) & 255));
		++index;
		return;
	}
	for(uint16_t i = 0; i < strip.numPixels(); ++i)
		blendPixel(i);
}

//---------------------------------------------- Color swing --------------------------------------------------------------
void swing::init(void) {
	len = 1;
	rnd = Random(2);
	uint32_t c = strip.wheel(Random(256));
	if (!rnd) {													// Use rainbow colors from Wheel
		w = Random(256);
		c = strip.wheel(w);
	}
	strip.setPixelColor(0, c);
	fwd			= true;
	next_color	= 0;
	index		= strip.numPixels() - len - 1;
}

void swing::show(void) {
	step();
	--index;

	if (index < 0) {
		COLOR c = strip.wheel(Random(256));
		if (!rnd) {
			w += 4;
			c = strip.wheel(w);
		}
		if (fwd)
			strip.setPixelColor(strip.numPixels() - len - 1, c);
		else
			strip.setPixelColor(len, c);
		++len;
		fwd = !fwd;
		index = strip.numPixels() - len - 1;
		if (len >= int(strip.numPixels())) {
			do_clear = true;									// Force the strip to be cleaned
			complete = true;
			return;
		}
	}
	complete = false;
}

//---------------------------------------------- Single color swing -------------------------------------------------------
void swingSingle::init(void) {
	len		= 1;
	w		= Random(256);
	color	= strip.wheel(w);
	fwd		= true;
	index	= len;
	strip.setPixelColor(0, color);
}

void swingSingle::show(void) {
	if (fwd) {
		for (int i = 0; i <= index; ++i) {
			if (i < (index - len))
				strip.setPixelColor(i, 0);
			else
				strip.setPixelColor(i, color);
		}
		++index;
		if (index >= int(strip.numPixels())) {
			fwd = false;
			len += Random(1, strip.numPixels() >> 4);
			index = strip.numPixels() - len - 1;
			w += 4;
		}
	} else {
		for (int i = strip.numPixels() - 1; i >= index; --i) {
			if (i > (index + len))
				strip.setPixelColor(i, 0);
			else
				strip.setPixelColor(i, color);
		}
		--index;
		if (index < 0) {
			fwd = true;
			index = len;
			w += 4;
		}
	}
	if (len >= int(strip.numPixels())) {
		do_clear = true;											// Force the strip to be cleaned
		complete = true;
		return;
	}
	complete = false;
}

//---------------------------------------------- Fill the strip by the color in aRandom order ----------------------------
void randomFill::init(void) {
	w			= Random(256);
	remain		= strip.numPixels();
	clr			= false;
	newDot(clr);
}

void randomFill::show() {
	int8_t val = 12;
	if (clr) val = -12;
	if (change(pos, val)) {
		if (remain <= 0) {
			if (clr) {
				strip.clear();
				w += 71;
				init();
				complete = true;
				return;
			} else {
				clr = true;
				remain = strip.numPixels();
				newDot(clr);
			}
		} else {
			newDot(clr);
		}
	}

	// Blend active pixels
	COLOR color = strip.wheel(w);
	for (uint16_t i = 0; i < strip.numPixels(); ++i) {
		if (i != pos && strip.getPixelColor(i) != 0) {
			strip.setPixelColor(i, color);								// Restore original color to disable huge deviation
			blendPixel(i, 10);
		}
	}
	complete = false;
}

void randomFill::newDot(bool clr) {
	COLOR color = 0;
	if (!clr) color = strip.wheel(w);

	uint8_t p = Random(remain);
	uint8_t c = 0;
	for (pos = 0; (pos < strip.numPixels()) && (c < p); ++pos) {
		if (!color) {
			while(strip.getPixelColor(pos) == 0) pos++;
			++c;
		} else {
			while(strip.getPixelColor(pos) != 0) pos++;
			++c;
		}
	}
	if (!color) {
		while(strip.getPixelColor(pos) == 0) pos++;
	} else {
		while(strip.getPixelColor(pos) != 0) pos++;
	}
	if (pos >= strip.numPixels()) {									// something is wrong in the code
		for (uint16_t i = 0; i < strip.numPixels(); ++i)
			strip.setPixelColor(i, color);
		remain = 0;
	}
	setColor(color);
	remain--;
}

//---------------------------------------------- Slow shining and flash by the different colors ---------------------------
void shineFlash::init(void) {
	w		= Random(256);
	mode	= 0;
	flash	= false;
	startNewColor();
}

void shineFlash::show() {
	int n = strip.numPixels();
	bool finish = true;
	switch(mode) {
    	case 0:														// Light up
    		finish = changeAll(4);
    		if (finish) {
    			flash  = true;
    			remain = Random(17, 30);
    			wait   = 0;
    		}
    		break;
    	case 1:														// Run flash
    		if (flash) {											// Lit the LED
    			if (--wait > 0) return;
    			wait = Random(2, 7);
    			indx = Random(n);
    			if (remain > 0) {
    				COLOR c = strip.getPixelColor(indx);
    				c |= 0x808080;
    				strip.setPixelColor(indx, c);
    				flash = false;
    				remain --;
    			} else {
    				mode ++;
    			}
    		} else {
    			COLOR c = strip.getPixelColor(indx);
    			c &= 0x7f7f7f;
    			strip.setPixelColor(indx, c);
    			flash = true;
    		}
    		finish = false;
    		break;
    	case 2:														// Fade out
    	default:
    		finish = changeAll(-4);
    		break;
	}

	if (finish) {													// The current color has been light fully
		++mode;
		if (mode >= 3) {
			startNewColor();
			mode = 0;
			complete = true;
			return;
		}
	}
	complete = false;
}

void shineFlash::startNewColor(void) {
	COLOR c = strip.wheel(w);
	c &= 0x7f7f7f;
	w += 17;
	setColor(c);
	c &= 0x10101;
	int  n = strip.numPixels();
	for (int16_t i = 0; i < n; ++i)
		strip.setPixelColor(i, c);
}

//---------------------------------------------- Show single wave moving in Random direction ------------------------------
void singleWave::init(void) {
	COLOR c = strip.wheel(Random(256));
	uint32_t r = c & 0xff;
	uint32_t g = (c >> 8)  & 0xff;
	uint32_t b = (c >> 16) & 0xff;
	for (uint8_t i = 1; i <= 4; ++i) {
		r >>= 1;
		g >>= 1;
		b >>= 1;
		uint32_t cc = b & 0xff; cc <<= 8;
		cc |= g & 0xff; cc <<= 8;
		cc |= r & 0xff;
		dot[i] = cc;
	}

	setColor(dot[3]);
	c &= 0x10101;
	uint16_t n = strip.numPixels();
	for (uint16_t i = 0; i < n; ++i)
		strip.setPixelColor(i, c);
	mode   = 0;
	pos    = Random(n);
	remain = Random(5, 15);
	stp    = 0;
}

void singleWave::show() {
	uint16_t n = strip.numPixels();
	bool finish = true;
	switch(mode) {
    	case 0:														// Light up
    		finish = changeAll(4);
    		break;
    	case 1:														// move the soliton
    		finish = false;
    		if (stp <= 0) {
    			incr = 1;
    			if (pos > int(n >> 1)) incr = -1;
    			int m = int(n) - pos - 2;
    			if (incr < 0) m = pos - 2;
    			stp = Random(5, m);
    			--remain;
    			if (remain <= 0) {
    				for (uint16_t i = 0; i < n; ++i)
    					strip.setPixelColor(i, dot[3]);
    				finish = true;
    				break;
    			}
    		}
    		pos += incr;
    		for (uint16_t i = 0; i < n; ++i) {
    			strip.setPixelColor(i, dot[3]);
    		}
    		changeAll(Random(9) - 4);
    		for (int16_t i = 3; i > 0; --i) {
    			if ((pos - i) >= 0) strip.setPixelColor(pos - i, dot[i]);
    		}
    		for (uint16_t i = 0; i <= 3; ++i) {
    			if ((pos + i) < n) strip.setPixelColor(pos + i, dot[i]);
    		}
    		stp --;
    		break;
    	case 2:														// Fade out
    	default:
    		finish = changeAll(-4);
    		break;
	}

	if (finish) {													// The current color has been light fully
		++mode;
		if (mode >= 3) {
			init ();
			complete = true;
			return;
		}
	}
	complete = false;
}

//---------------------------------------------- Several worms are moving Randomly ---------------------------------------
void worms::init(void) {
	active   = 0;
	add();
}

void worms::show(void) {
	int16_t n = strip.numPixels();

	// fade away
	changeAll(-32);

	// Move existing
	for (uint8_t wi = 0; wi < active; ++wi) {
		int np = w[wi].pos - 1;
		if (w[wi].fwd) np += 2;
		if ((np < 0) || (np >= n)) {
			die(wi);
			--wi;
			continue;
		}
		COLOR c = strip.getPixelColor(np);
		if ((c != 0) && (Random(10) == 0)) {
			die(wi);
			--wi;
			continue;
		} else {
			c = TWOCLR::add(c, w[wi].color);
			w[wi].pos = np;
			strip.setPixelColor(np, c);
		}
	}

	if (Random(12) == 0) add();
}

void worms::add(void) {
	if (active >= 5) return;

	uint8_t mode = Random(3);
	int n = strip.numPixels();
	switch (mode) {
    	case 0:														// Run from the start
    		w[active].pos = 0;
    		break;
    	case 1:														// Run from the end
    		w[active].pos = n-1;
    		break;
    	case 2:														// Run from the aRandom position
    	default:
    		w[active].pos = Random(n);
    		break;
	}
	w[active].color = strip.wheel(Random(256));
	if (strip.getPixelColor(w[active].pos) != 0) return;
	if (w[active].pos < n/3) {
		w[active].fwd = true;
	} else if ((n - w[active].pos) < n/3) {
		w[active].fwd = false;
	} else {
		w[active].fwd = Random(2);
	}
	++active;
}

void worms::die(uint8_t index) {
	--active;
	w[index].color = w[active].color;
	w[index].pos   = w[active].pos;
	w[index].fwd   = w[active].fwd;
}

//---------------------------------------------- Show interferention from many sources ----------------------------------
void interfer::init(void) {
	active	= 0;
	tm		= 0;
	add();
}

void interfer::show(void) {
	int n = strip.numPixels();

	for (int i = 0; i < n; ++i) {
		COLOR c = 0;
		for (uint8_t j = 0; j < active; ++j) {
			COLOR c1 = clr(i, j);
			c = TWOCLR::add(c, c1);
		}
		strip.setPixelColor(i, c);
	}
	++tm;
	if (tm %64 == 0) {
		for (uint8_t i = 0; i < active; ++i)
			w[i] += 4;
	}

	if (!Random(20)) add();
}

void interfer::add(void) {
	if (active >= num_inter) return;
	pos[active]		= Random(strip.numPixels());
	w[active]		= Random(256);
	start[active]	= tm;
	active++;
}

uint32_t interfer::clr(int p, uint8_t source) {
	uint32_t c = 0;
	int s_pos = pos[source];
	int e  = tm - start[source];
	e -= abs(p - s_pos);
	if (e < 0) return c;											// The wave is not here yet
	e %= 64;														// The wave period
	uint8_t elm = 0;
	if (e < 32)														// Half way
		elm = (31 - e) << 3;
	else
		elm = (e - 64) << 3;

	uint32_t color = strip.wheel(w[source]);
	for (uint8_t i = 0; i < 3; ++i) {
		int max_c = (color >> (8*i)) & 0xff;
		max_c -= elm;
		if (max_c < 0) max_c = 0;
			uint32_t nc = max_c;
			nc <<= 8*i;
			c |= nc;
	}
	return c;
}

//------------------------------------------- Random  colors toward each other ------------------------------------------
void toward::show(void) {
	int n = strip.numPixels();
	for (int i = n-2; i >= 2; i -= 2) {
		COLOR c = strip.getPixelColor(i-2);
		strip.setPixelColor(i, c);
	}
	for (int i = 1; i < n-1; i += 2) {
		COLOR c = strip.getPixelColor(i+2);
		strip.setPixelColor(i, c);
	}
	if (Random(16) == 0) {
		strip.setPixelColor(0,   0);
		strip.setPixelColor(n-1, 0);
		w += 4;
	} else {
		strip.setPixelColor(0,   strip.wheel(w+128));
		strip.setPixelColor(n-1, strip.wheel(w));
	}
}

//------------------------------------------- Raindow colors toward each other ------------------------------------------
void towardRain::show(void) {
	int n = strip.numPixels();
	for (int i = n-2; i >= 2; i -= 2) {
		COLOR c = strip.getPixelColor(i-2);
		strip.setPixelColor(i, c);
	}
	for (int i = 1; i < n-1; i += 2) {
		COLOR c = strip.getPixelColor(i+2);
		strip.setPixelColor(i, c);
	}

	if (!Random(17)) {
		strip.setPixelColor(0,   0);
		strip.setPixelColor(n-1, 0);
	} else {
		strip.setPixelColor(0,   strip.wheel(w)); w += 9;
		strip.setPixelColor(n-1, strip.wheel(w)); w += 3;
	}
}

//---------------------------------------------- Slow single wave moving in aRandom direction -- ---------------------------
void lghtHouse::init(void) {
	COLOR c = 0xff;
	for (uint8_t i = 0; i <= 4; ++i) {
		dot[i] = c | (c << 8) | (c << 16);
		c >>= 1;
	}
	setColor(dot[4]);
	pos    = Random(strip.numPixels());
	stp    = 0;
	sp     = Random(1, 4);
	dlay   = sp;
}

void lghtHouse::show() {
	changeAll(-8);
	if (--dlay > 0) return;
	dlay = sp;

	int n = strip.numPixels();
	if (stp <= 0) {
		incr = 1;
		if (pos > n / 2) incr = -1;
		stp = Random(5, n);
		sp  += Random(3) - 1;
		if (sp < 1)
			sp = 1;
		else if (sp > 3)
			sp = 3;
	}
	pos += incr;
	pos %= n;

	for (int i = 0; i <= 5; ++i) {
		uint8_t indx = i;
		if (indx >= 1) indx --;
		int x = pos + i;
		if (x >= n)
			x -= n;
		else if (x < 0)
			x += n;
		strip.setPixelColor(x, dot[indx]);
		x = pos - i;
		if (x >= n)
			x -= n;
		else if (x < 0)
			x += n;
		strip.setPixelColor(x, dot[indx]);
	}
	stp --;
}

//---------------------------------------------- Random dropped color dot fading out from epicenter ----------------------
void rndDrops::show(void) {
	int n = strip.numPixels();
	for (uint8_t i = 0; i < num; ++i) {
		if (++dr[i].tm > 7) {											// Delete old drops
			dr[i].pos = dr[uint8_t(num-1)].pos;
			dr[i].tm  = dr[uint8_t(num-1)].tm;
			--num; --i;
			continue;
		}
		int p = dr[i].pos - dr[i].tm;
		if (p < 0) p += n;
		COLOR c1 = strip.getPixelColor(p+1);
		change(p+1, -64);
		COLOR c2 = strip.getPixelColor(p);
		c2 = TWOCLR::add(c1, c2);
		strip.setPixelColor(p, c2);

		p = dr[i].pos + dr[i].tm;
		if (p >= n) p -= n;
		c1 = strip.getPixelColor(p-1);
		if (dr[i].tm > 1) change(p-1, -32);
		c2 = strip.getPixelColor(p);
		c2 = TWOCLR::add(c1, c2);
		strip.setPixelColor(p, c2);

		change(dr[i].pos, -64);
	}

	changeAll(-32);
	add();
}

void rndDrops::add(void) {
	if (num >= 16) return;
	int pos	= Random(strip.numPixels());
	COLOR c = strip.getPixelColor(pos);
	if (c) return;
	c = strip.wheel(Random(256));
	strip.setPixelColor(pos, c);
	dr[uint8_t(num)].pos = pos;
	dr[uint8_t(num)].tm  = 0;
	num++;
}

// --------------------------------------------- Solitons are creaping up or down -----------------------------------------
void solCreep::init(void) {
	w	= Random(256);
	fwd = Random(2);
	change_direction = Random(200, 500);
	newSoliton();
}

void solCreep::show(void) {
	step();

	if (--change_direction <= 0) {
		fwd = !fwd;
		change_direction = Random(200, 500);
	}

	int pos = strip.numPixels() - 1;
	if (fwd) pos = 0;

	if (sol <= 4) {
		int i = abs(sol);
		strip.setPixelColor(pos, dot[i]);
		++sol;
	} else {
		if (--space >= 0) {
			strip.setPixelColor(pos, 0);
		} else {
			newSoliton();
		}
	}
}

void solCreep::newSoliton(void) {
	sol = -3;
	COLOR c = strip.wheel(w);
	dot[0] = c;
	w += 71;
	uint32_t r = c & 0xff;
	uint32_t g = (c >> 8)  & 0xff;
	uint32_t b = (c >> 16) & 0xff;
	for (uint8_t i = 1; i <= 4; ++i) {
		r >>= 1;
		g >>= 1;
		b >>= 1;
		COLOR cc = b & 0xff; cc <<= 8;
		cc |= g & 0xff; cc <<= 8;
		cc |= r & 0xff;
		dot[i] = cc;
	}
	space = Random(3, 10);

	int16_t pos = strip.numPixels() - 1;
	if (fwd) pos = 0;
	strip.setPixelColor(pos, dot[4]);
}

// --------------------------------------------- Theatre-style crawling lights from neopixel example ----------------------
void theatChase::init(void) {
	w				= Random(256);
	color_period	= Random(5, 20);
	color_shift		= color_period;
	stp = 0;
}

void theatChase::show(void) {
	int n = strip.numPixels();
	for (int16_t i = 0; i < n; i += 3)
		strip.setPixelColor(i + stp, 0);								// turn off previous state pixels

	if (++stp >= 3) stp = 0;
	COLOR color = strip.wheel(w);
	for (int i = 0; i < n; i += 3)
		strip.setPixelColor(i + stp, color);
	if (--color_shift <= 0) {
		color_shift = color_period;
		++w;
	}
}

// --------------------------------------------- Meteors falling down -----------------------------------------------------
void meteorSky::init(void) {
	uint16_t n	= strip.numPixels();
	head		= Random(n - (n >> 2), n);								// Select starting position of the meteor head
	tail		= head;
	grow 		= true;													// The brightness should grow first
	complete	= false;
	COLOR c     = 0xffffff;												// Pure white
	if (Random(2)) {													// Generate white color with probability 50%
		c   = strip.lightWheel(Random(256));
	}
	setColor(c);														// Grow color to specified one
	changeClr(c, -32);													// Select starting color of the pixel
	strip.setPixelColor(head, c);
	do_clear	= false;
}

void meteorSky::show(void) {
	uint16_t fade = head;
	COLOR c = strip.getPixelColor(head);
	if (grow) {
		if (c && head > 0) {
			grow = !changeClr(c, 16);									// Increase brightness
			strip.setPixelColor(--head, c);
		}
		++fade;															// Start fade next pixel
	} else {
		if (c && head > 0) {
			strip.setPixelColor(--head, c);
		}
	}

	// Fade pixels in the tail
	for (int16_t i = tail; i >= int16_t(fade); --i) {
		c = strip.getPixelColor(i);
		changeClr(c, -80);
		strip.setPixelColor(i, c);
		if (c == 0) {
			tail = i;
		}
	}
	complete = (c == 0) && (head == tail);

	if (complete && Random(20) == 0) init();
}


// --------------------------------------------- Symmetrical dots run -----------------------------------------------------
void symmRun::init(void) {
	w = Random(256);													// Select random color of the pixels
	COLOR c 	= strip.lightWheel(w);
	left		= 0;
	right		= strip.numPixels() - 1;
	complete	= false;
	phase		= 0;
	setColor(c);
	do_clear	= false;
}

void symmRun::show(void) {
	bool 		done = true;											// Whether we reach the required color
	bool		dark = true;											// Whether we fade all the pixels
	uint16_t 	n	 = strip.numPixels();

	switch (phase) {
		case 0:
			// Increase the brightness of two current pixels
			if (left < right) {
				if (!change(left,  70)) done = false;
				if (!change(right, 70)) done = false;
			}

			if (done && (left < right)) {								// The current pixels have been reached required color
				++left;
				--right;
			}
			if (done && dark && (left > right)) {
				phase	= 1;											// Activate the second phase, fade the pixels
				left	= 0;
				right	= strip.numPixels() - 1;
				drk_stp = 0;
			}
			return;

		case 1:															// The second phase, fade out the tail
			// Fade the left and right parts
			for (uint16_t i = 0; i < left; ++i)							// Fade left part
				if (!change(i, -2)) dark = false;
			for (uint16_t i = right+1; i < n; ++i)
				if (!change(i, -2)) dark = false;
			if (++drk_stp >= 10) {
				drk_stp = 0;
				if (left+2 < right) {
					++ left;
					--right;
				} else {
					if (dark) {
						phase = 2;
						setColor(0xffffff);
					}
				}
			}
			return;

		case 2:															// The second phase, boom
			if (!change(left,    1)) done = false;
			if (!change(right,   1)) done = false;
			if (done)
				phase = 3;
			return;

		case 3:															// The third phase, gone away
			strip.setPixelColor(left,  0);
			strip.setPixelColor(right, 0);
			if (left > 0) {
				-- left;
				++right;
				COLOR c = strip.lightWheel(w);							// Our color
				strip.setPixelColor(left,  c);
				strip.setPixelColor(right, c);
			} else {
				phase 		= 4;
				complete	= true;
			}
			return;

		case 4:															// Wait for new sequence
		default:
			if (Random(10) == 0) init();
			return;
	}

}

// --------------------------------------------- Defined Meteors falling down fast ----------------------------------------
void metSingle::init(void) {
	uint16_t n	= strip.numPixels();
	head		= Random(n - (n >> 2), n);								// Select starting position of the meteor head
	complete	= false;
	do_clear	= false;
}

void metSingle::show(void) {
	if (complete) {
		if (Random(20) == 0) init();
		return;
	}

	uint16_t n	= strip.numPixels();
	if (uint16_t(head)+clr_size+1 < n)
		strip.setPixelColor(head+clr_size+1, 0);
	for (uint16_t i = 0; i < clr_size; ++i) {
		if (head + i < n) {
			strip.setPixelColor(head+i, clr[i]);
		}
	}
	if (head > 0) {
		--head;
	} else {
		strip.clear();
		complete = true;
	}
}

// --------------------------------------------- Classical multicolor strip simulation ------------------------------------
void pureStrip::init(void) {
	mode		= Random(4);
	if (mode > 2) {														// Fast modes
		min_p = 50; max_p = 100;
	} else {															// Slowly modes
		min_p = 1;  max_p = 5;
	}

	num_color	= Random(3, 7);											// 3-6 different colors in the strip
	if (Random(5) == 0) {												// Monochrome strip
		COLOR c = strip.wheel(Random(256));
		for (uint8_t i = 0; i < 6; ++i)
			clr[i] = c;
	} else {															// Multicolor strip
		uint8_t	available[6];											// Generate available color codes. This make sure each color used just obce
		for (uint8_t i = 0; i < 6; ++i)
			available[i] = i+1;
		for (uint8_t i = 0; i < 50; ++i) {								// Shuffle the color codes
			uint8_t pos = Random(6);
			if (pos) {
				uint8_t tmp 	= available[0];
				available[0]	= available[pos];
				available[pos]  = tmp;
			}
		}

		for (uint8_t i = 0; i < num_color; ++i) {						// Define the colors
			uint8_t code	= available[i];
			clr[i]			= 0;
			if (code & 1)	clr[i] |= 0xff;								// Whole blue
			if (code & 2)	clr[i] |= 0xff00;							// Whole green
			if (code & 4)	clr[i] |= 0xff0000;							// Whole red
		}
	}
	stage		= num_color-1;
	blink		= 0;
	complete	= true;													// Can be interrupted any time
	do_clear	= true;													// Clear the strip after the end of sequence
}

void pureStrip::show(void) {
	uint16_t n		= strip.numPixels();
	bool     done	= true;
	uint8_t  nxt 	= stage + 1;
	if (nxt >= num_color) nxt = 0;

	switch (mode) {
		case 0:															// Slowly increment brightness, slowly date out
			for (uint16_t i = stage; i < n; i += num_color)
				change(i, -8);
			setColor(clr[nxt]);
			for (uint16_t i = nxt;   i < n; i += num_color)
				if (!change(i, 8)) done = false;
			break;

		case 1:															// Switch on colors one by one, then switch them off
			setColor(clr[nxt]);
			for (uint16_t i = nxt;   i < n; i += num_color) {
				if (blink < num_color) {
					if (!change(i, 8))  done = false;
				} else {
					if (!change(i, -8)) done = false;
				}
			}
			if (done) {
				if (++blink >= (num_color << 1))
					blink = 0;
			}
			break;

		case 2:															// All on then all off
			for (uint8_t j = 0; j < num_color; ++j) {
				setColor(clr[j]);
				for (uint16_t i = j; i < n; i += num_color) {
					if (!blink) {
						if (!change(i, 4))  done = false;
					} else {
						if (!change(i, -4))	done = false;
					}
				}
			}
			if (done) blink = !blink;
			break;

		case 3:															// Simple sequential switch
		default:
			for (uint16_t i = stage; i < n; i += num_color)
				strip.setPixelColor(i, 0);
			for (uint16_t i = nxt;   i < n; i += num_color)
				strip.setPixelColor(i, clr[nxt]);
			break;
	}
	if (done) stage = nxt;
}

void sideFill::init(void) {
	w			= Random(256);
	fwd 		= Random(2);
	rainbow		= Random(2);
	stage		= 1;													// Current maximum length of lit leds
	index		= 0;													// The index of led that should be lit in this step
	on			= true;
	complete	= false;
	do_clear	= true;
}

void sideFill::show(void) {
	if (complete) init();

	uint16_t n			= strip.numPixels();
	uint16_t led_index	= index;
	if (!fwd)
		led_index = n - index -1;										// If we fill the strip from the back, fix the led position

	if (on) {															// switch on new pixel
		COLOR c = strip.wheel(w);
		if (rainbow) ++w;
		strip.setPixelColor(led_index, c);
		if (++index >= stage) {											// The stage is finished
			--index;
			on = false;													// Start to clear the pixels
		}
	} else {															// clear off new pixel
		strip.setPixelColor(led_index, 0);
		if (index > 0) {
			--index;
		} else {														// Start the next stage
			on = true;
			uint16_t incr = 1;
			if (n > stage + 1) incr = Random(1, ((n-stage+1) >> 1) + 1);
			if (incr > 10) incr = 10;
			stage += incr;
			if (stage >= n) {											// All the string was filled up, change the direction
				complete = true;
			}
		}
	}
}

// --------------------------------------------- Browian Motion with the tail --------------------------------------------
void browMotion::init(void) {
	uint16_t n	= strip.numPixels();
	pos			= Random(n+1);
	newDestination(n);
	complete	= true;
	do_clear	= true;
}

void browMotion::show(void) {
	changeAll(-12);
	COLOR		c = strip.wheel(w);
	uint16_t	n = strip.numPixels();
	if (speed > 0) {													// Go forward
		for (uint8_t i = 0; i <= speed; ++i) {
			strip.setPixelColor(pos, c);
			if (pos == destination) {
				newDestination(n);
				return;
			}
			if (pos < n+1) {
				++pos;
			} else {
				speed = speed * (-1);
			}
		}
	} else {															// Go backward
		for (uint8_t i = 0; i <= abs(speed); ++i) {
			strip.setPixelColor(pos, c);
			if (pos == destination) {
				newDestination(n);
				return;
			}
			if (pos > 0) {
				--pos;
			} else {
				speed = speed * (-1);
			}
		}
	}
}

void browMotion::newDestination(uint16_t num_pixels) {
	w 			= Random(256);
	destination	= Random(num_pixels+1);
	speed		= (int16_t(destination) - int16_t(pos)) * 10 / num_pixels;
	speed		= constrain(speed, 1, 3);
}


// --------------------------------------------- Rain drops running down -------------------------------------------------
void rainDrops::init(void) {
	active_drops	= 0;
	complete		= true;
	do_clear		= true;
	newDrop();
}

void rainDrops::show(void) {
	changeAll(-24);
	for (uint8_t d = 0; d < active_drops; ++d) {
		for (uint8_t s = 0; s <= drop[d].speed; ++s) {
			if (drop[d].head < s) {
				drop[d].c	= 0;										// Mark the drop to be deleted
				break;
			}
			strip.setPixelColor(drop[d].head-s, drop[d].c);
		}
		if (drop[d].head >= drop[d].speed) {
			drop[d].head -= drop[d].speed;
		}
		changeClr(drop[d].c, -16);
	}

	// Remove inactive drops
	for (int8_t d = active_drops-1; d >= 0; --d) {
		if (drop[d].c == 0) {
			drop[d].c 		= drop[active_drops-1].c;
			drop[d].speed	= drop[active_drops-1].speed;
			drop[d].head	= drop[active_drops-1].head;
			if (--active_drops == 0) break;
		}
	}

	// Create new Drops
	if (active_drops < max_drops && Random(10) == 0) {
		newDrop();
	}
}

void rainDrops::newDrop(void) {
	if (active_drops >= max_drops)
		return;
	uint16_t n	= strip.numPixels();
	drop[active_drops].head		= Random(n >> 1, n+1);
	drop[active_drops].c		= strip.wheel(Random(256));
	drop[active_drops].speed	= Random(1, 4);
	++active_drops;
}

// --------------------------------------------- Ripe fruits booms and run two pieces ------------------------------------
void ripeFruit::init(void) {
	active_fruits	= 0;
	complete		= true;
	do_clear		= true;
	newFruit();
}

void ripeFruit::show(void) {
	uint16_t n	= strip.numPixels();
	for (uint16_t i = 0; i < n; ++i) {									// Fade out all pixels except the active one
		bool skip = false;
		for (uint8_t f = 0; f < active_fruits; ++f) {
			if ((fruit[f].speed == 0) && (i == fruit[f].part[0])) {
				skip = true;
				break;
			}
		}
		if (!skip) change(i, -16);
	}

	for (uint8_t f = 0; f < active_fruits; ++f) {
		if (fruit[f].speed == 0) {										// The fruit is getting ready
			setColor(fruit[f].c);
			if (change(fruit[f].part[0], 2)) {							// The fruit is ripen
				strip.setPixelColor(fruit[f].part[0], 0xffffff);
				fruit[f].part[1] = fruit[f].part[0] + 1;
				fruit[f].speed	= Random(1, 3);							// Start moving
			}
		} else {														// The fruit parts are moving
			for (uint8_t s = 0; s <= fruit[f].speed; ++s) {
				if (fruit[f].part[0] >= s) {
					strip.setPixelColor(fruit[f].part[0]-s, fruit[f].c);
				}
				if (fruit[f].part[1] + s < n) {
					strip.setPixelColor(fruit[f].part[1]+s, fruit[f].c);
				}
			}

			if (fruit[f].part[0] >= fruit[f].speed) {
				fruit[f].part[0] -= fruit[f].speed;
			} else {
				fruit[f].c	= 0;										// Mark to be deleted
			}
			if (fruit[f].part[1] +  fruit[f].speed < n) {
				fruit[f].part[1] += fruit[f].speed;
			} else {
				fruit[f].c	= 0;										// Mark to be deleted
			}

		}
		changeClr(fruit[f].c, -16);
	}
	// Remove inactive fruits
	for (int8_t f = active_fruits-1; f >= 0; --f) {
		if (fruit[f].c == 0) {
			fruit[f].c 			= fruit[active_fruits-1].c;
			fruit[f].speed		= fruit[active_fruits-1].speed;
			fruit[f].part[0]	= fruit[active_fruits-1].part[0];
			fruit[f].part[1]	= fruit[active_fruits-1].part[1];
			if (--active_fruits == 0) break;
		}
	}

	// Create new Fruit
	if (active_fruits < max_fruits && Random(5) == 0) {
		newFruit();
	}
}

void ripeFruit::newFruit(void) {
	if (active_fruits >= max_fruits)
		return;
	uint16_t n	= strip.numPixels();
	fruit[active_fruits].part[0]= Random(4, n - 5);
	fruit[active_fruits].part[1]= n;									// Not active yet
	fruit[active_fruits].c		= strip.wheel(Random(256));
	fruit[active_fruits].speed	= 0;
	++active_fruits;
}

// --------------------------------------------- The single color that changes the brightness as a sine ------------------
void brightWave::init(void) {
	w			= Random(256);
	t			= 0;
	fwd			= Random(2);
	complete	= true;
	do_clear	= true;
}

void brightWave::show(void) {
	COLOR color = strip.wheel(w);
	for (uint16_t i = 0; i < strip.numPixels(); ++i) {
		COLOR c = intencity(color, i+t);
		strip.setPixelColor(i, c);
	}
	if (fwd) --t; else ++t;
	if (Random(10) == 0) {
		++w;
		if (Random(30) == 0)
			fwd = !fwd;
	}
}

// --------------------------------------------- Color pixels walking up and down with sine brightness -------------------
void brColCreep::init(void) {
	fwd			= Random(2);
	w			= Random(256);
	t			= Random(64);
	complete	= true;
	do_clear	= true;
}

void brColCreep::show(void) {
	step();
	uint16_t	pos	= strip.numPixels() - 1;
	if (fwd)	pos	= 0;
	COLOR		c	= strip.wheel(++w);
	c				= intencity(c, t);
	if (++t >= 64)	t = 0;
	strip.setPixelColor(pos, c);
}
