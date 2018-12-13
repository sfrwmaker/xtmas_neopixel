#ifndef __CLRUTILS_H
#define __CLRUTILS_H
#include "neopixel.h"
#include "Random.h"

extern NEOPIXEL		strip;

// --------------------------------------------- creep the sequence up or down, superclass --------------------------------
class CRAWL {
	public:
		CRAWL(void)                                           		{ }
		void		step(void);
	protected:
		bool		fwd			= true;								// direction to crawl: false - backward, true - forward
		COLOR		next_color	= 0;
};

//---------------------------------------------- Brightness manipulation --------------------------------------------------
class BRGTN {
	public:
    	BRGTN()                                               		{ }
    	void		setColor(uint32_t c);
    	bool		changeClr(COLOR& c, int8_t val);
    	bool		change(uint16_t index, int8_t val);
    	bool		changeAll(int8_t val);
	protected:
    	uint8_t		color[3];
};

//---------------------------------------------- Blend manipulations ------------------------------------------------------
class BLEND {
	public:
    	BLEND()														{ }
    	COLOR		add(COLOR color1, COLOR color2);
    	COLOR		sub(COLOR color1, COLOR color2);
    	void		blendPixel(uint16_t p, uint8_t deviation = 20);
};

//---------------------------------------------- Color superposition class ------------------------------------------------
class TWOCLR {
	public:
    	TWOCLR()													{ }
    	COLOR add(COLOR c1, COLOR c2);
};

//---------------------------------------------- The sine wave value class ------------------------------------------------
class WAVE {
	public:
    	WAVE()														{ }
    	uint8_t	amplitude(uint8_t n);								// sin(PI*n/32) * 127 + 127
    	COLOR	intencity(COLOR c, uint8_t n);
	private:
    	const	uint8_t sine[17]	= { 127, 139, 152, 164, 176, 187, 198, 208, 217, 225, 233, 239, 244, 249, 252, 253, 254 };
};

#endif
