#ifndef __CLEAN_H
#define __CLEAN_H
#include "neopixel.h"
#include "Random.h"

extern NEOPIXEL		strip;

//---------------------------------------------- Classes for strip clearing  ----------------------------------------------
class clr  {
	public:
    	clr(void)													{ complete = false; }
    	virtual 		~clr(void)									{ }
    	virtual void	init(void) = 0;
    	virtual void	show(void) = 0;
    	bool			isComplete(void);
    	bool			fade(uint16_t index, uint8_t val);
    	bool			fadeAll(uint8_t val);
	protected:
    	bool		complete;
};

// --------------------------------------------- Clear the strip from the either side -------------------------------------
class clearSide : public clr {
	public:
    	clearSide(void)												{ index = 0; fwd = true; color = 0; }
    	virtual void 	init(void);
    	virtual void 	show(void);
	private:
    	COLOR		color;
    	int			index;
    	bool		fwd;
};

// --------------------------------------------- Clear the strip from the center to both ends -----------------------------
class clearCntr : public clr {
	public:
    	clearCntr(void)												{ color = 0; l = r = 0; fwd = true; }
    	virtual void	init(void);
    	virtual void	show(void);
	private:
    	COLOR		color;
    	int			l, r;
    	bool		fwd;
};


// --------------------------------------------- Clear the strip from the ether side --------------------------------------
class clearFade : public clr {
	public:
    	clearFade()													{ decrement = 0; }
    	virtual void	init(void)									{ complete = false; decrement = Random(4, 16); }
    	virtual void	show(void)									{ complete = clr::fadeAll(decrement); }
	private:
    	uint8_t		decrement;
};

// --------------------------------------------- Clear the strip by 'eating' the pixels from the center -------------------
class eatCntr : public clr {
	public:
    	eatCntr()													{ remain = 0; }
    	virtual void	init(void)									{ complete = false; remain = strip.numPixels()/2 + 1;}
    	virtual void	show(void);
	private:
    	int			remain;
};

//---------------------------------------------- Clear the strip by dividing it by 2 ------------------------------------
class clearHalf :  public clr {
	public:
    	clearHalf()													{ one_step = 0; }
    	virtual void	init(void);
    	virtual void	show(void);
	private:
    	int			one_step;
};



#endif
