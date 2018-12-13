#ifndef __MANAGER_H
#define __MANAGER_H
#include "animation.h"
#include "clean.h"

//---------------------------------------------- Shuffle the animation in the random order --------------------------------
class shuffle {
	public:
    	shuffle(uint8_t a_size);
    	uint8_t		next(void);
	private:
    	void		randomize(void);
    	uint8_t		*index;											// The array of animations, allocated by new()
    	uint8_t		num_anim;										// The active animation number
    	uint8_t		curr;
};

// --------------------------------------------- The sequence manager -----------------------------------------------------
class MANAGER : public shuffle {
	public:
    	MANAGER(animation* a[], uint8_t a_size, clr* c[], uint8_t clr_size);
    	void		init(void);
    	void		show(void);
    	void        menu(void)                              { stp_period --; if (stp_period < 1) stp_period = 1; }
    	void        menu_l(void)                            { initClear(); }
    	void        incr(void)                              { stp_period ++; if (stp_period > 20) stp_period = 20; }
	private:
    	void		initClear(void);
    	bool		isClean(void);
    	animation**	anims;
    	clr**		clearance;
    	uint8_t		num_clr;
    	uint32_t	stp;
    	uint16_t	stp_period			= 0;
    	uint16_t	clr_stp_period		= 0;
    	uint32_t	next				= 0;						// The time for the next animation, ms
    	uint8_t		aIndex				= 0;						// Current animation index
    	animation*  a 					= 0;
    	clr*		c 					= 0;
    	bool		do_clear;										// Whether cleaning the strip
};

#endif
