#ifndef __ANIMATION_H
#define __ANIMATION_H
#include "clrutils.h"

const uint8_t		min_time     = 30;							// Minimal sequence show time (seconds)

//---------------------------------------------- Base animation class with useful functions -------------------------------
class animation {
	public:
    	animation(void);
    	virtual		~animation(void)							{ }
    	virtual		void init(void) = 0;
    	virtual		void show(void) = 0;
    	void		getLimits(uint8_t& mi_p, uint8_t& ma_p)		{ mi_p = min_p; ma_p = max_p; }
    	uint8_t		min_p;										// The minimum period in tenth of second to show the stage
    	uint8_t		max_p;										// The maximum period in tenth of second to show the stage
    	uint8_t		show_time;									// The minimum time to show whole the sequence in 10-seconds intervals
    	bool		do_clear;									// Whether the strip have to be cleaned for the next loop
    	bool		complete;									// Whether the animation can be changed to the next one
};

// --------------------------------------------- Fill the dots one after the other with a color ---------------------------
class colorWipe: public animation {
	public:
    	colorWipe(void)											{ w = Random(256); index = 0; fwd = true; }
    	virtual void	init(void);
    	virtual void	show(void);
	private:
    	uint8_t 		w;
    	int				index;
    	bool  			fwd;
};

// --------------------------------------------- Walk the dots one after the other with a color ---------------------------
class colorWalk: public animation {
	public:
    	colorWalk(void)											{ min_p = 4; max_p = 12; index = 0; period = 0; fwd = true; w = 0; }
    	virtual void 	init(void);
    	virtual void 	show(void);
	private:
    	int   			index;
    	uint8_t  		period;
    	bool			fwd;
    	uint8_t			w;
};

// --------------------------------------------- creep the aRandom sequence up or down -------------------------------------
class randomCreep: public animation, public CRAWL {
  	  public:
    	randomCreep(void)										{ min_p = 6; max_p = 24; space = cnt = 0; change_direction = 0;}
    	virtual void 	init(void);
    	virtual void 	show(void);
  	  private:
    	uint8_t 		space;									// space between the color dots
    	int  			change_direction;
    	uint8_t 		cnt;
};

// --------------------------------------------- show the color Wave using rainbowCycle -----------------------------------
class colorWave: public animation, public CRAWL, public BRGTN {
  	  public:
    	colorWave(void)											{ index = 0; rdy = false; }
    	virtual void	init(void);
    	virtual void	show(void);
  	  private:
    	uint8_t			index;
    	bool			rdy;
};

// --------------------------------------------- show the rainbow (from the NEOPIXEL example) -----------------------------
class rainbow: public animation, public BRGTN {
	public:
    	rainbow(void)											{ index = 0; rdy = false; }
    	virtual void 	init(void)								{ index = 0; rdy = false; }
    	virtual void 	show(void);
	private:
    	uint8_t			index;
    	bool			rdy;
};

// --------------------------------------------- show the rainbowCycle (from the NEOPIXEL example) ------------------------
class rainCycle: public animation, public BRGTN {
	public:
    	rainCycle(void)											{ index = 0; rdy = false; }
    	virtual void	init(void)								{ index = 0; rdy = false; }
    	virtual void	show(void);
	private:
    	uint8_t			index;
    	bool 			rdy;
};

// --------------------------------------------- show the rainbow colors on the entire strip ------------------------------
class rainFull: public animation, public BRGTN {
	public:
    	rainFull(void)											{ min_p = 6; max_p = 24; index = 0; rdy = false; }
    	virtual void	init(void)								{ index = Random(256); rdy = false; }
    	virtual void	show(void);
	private:
    	uint8_t			index;
    	bool			rdy;
};

//---------------------------------------------- Light up with the Random color than fade out -----------------------------
class lightUp : public animation, public BRGTN {
	public:
    	lightUp()												{ min_p = 6; max_p = 24; show_time = 12; sp = inc = 0; }
    	virtual void	init(void)								{ sp = 1; inc = sp; newColor(); }
    	virtual void	show(void);
	private:
    	void			newColor(void);
    	uint8_t			sp;
    	int8_t			inc;
};

//---------------------------------------------- aRandom sparcs ------------------------------------------------------------
class sparks : public animation, public BRGTN {
	public:
    	sparks()												{ min_p = 2; max_p = 6; show_time = 12; }
    	virtual void	init(void)								{ for (uint8_t i = 0; i < 8; ++i) pos[i] = 0; }
    	virtual void	show(void);
	private:
    	uint16_t		pos[8];
};

//---------------------------------------------- aRandom sparks fade out ---------------------------------------------------
class rndFade : public animation, public BRGTN {
	public:
    	rndFade()												{ min_p = 6; max_p = 24; show_time = 12; }
    	virtual void	init(void)								{ }
    	virtual void 	show(void);
};

//---------------------------------------------- Lights run from the center -----------------------------------------------
class centerRun : public animation {
	public:
    	centerRun()												{ min_p = 4; max_p = 20; m = l = r = 0; color = 0; }
    	virtual void	init(void);
    	virtual void	show(void);
	private:
    	COLOR 			color;
    	int16_t			m, l, r;
};

//---------------------------------------------- Slow shining by the different colors -------------------------------------
class shineSeven :  public animation, public BRGTN {
	public:
    	shineSeven()											{ min_p = 4; max_p = 20; show_time = 12; curs = w = base = 0; }
    	virtual void	init(void);
    	virtual void	show(void);
	private:
    	void			startNewColor(void);
    	uint8_t			curs;
    	uint8_t			w;
    	uint8_t			base;
};

//---------------------------------------------- Rapid walking by the different colors -----------------------------------
class walkSeven : public animation, public BRGTN, public TWOCLR {
	public:
    	walkSeven()                                           { min_p = 8; max_p = 15; show_time = 9; curs = w = ch_dir = period = 0; fwd = true; }
    	virtual void 	init(void);
    	virtual void 	show(void);
	private:
    	int8_t			curs;
    	uint8_t			w;
    	bool 			fwd;
    	int  			ch_dir;
    	uint8_t			period;
};

//---------------------------------------------- Rapid flashing by the difference colors ---------------------------------
class flashSeven : public animation, public BRGTN {
	public:
    	flashSeven()											{ min_p = 4; max_p = 8; show_time = 9; curs = w = ch_dir = period = 0; fwd = true; }
    	virtual void	init(void);
    	virtual void	show(void);
	private:
    	int8_t			curs;
    	uint8_t			w;
    	bool			fwd;
    	int				ch_dir;
    	uint8_t			period;
};

//---------------------------------------------- Slow merging of two colors -----------------------------------------------
class mergeOne : public animation, public TWOCLR {
	public:
    	mergeOne()												{ min_p = 8; max_p = 30; cl = cr = 0; l = r = 0; }
    	virtual void	init(void);
    	virtual void	show(void);
	private:
    	COLOR			cl, cr;
    	int16_t			l, r;
};

//---------------------------------------------- Fast merging of Waves ----------------------------------------------------
class mergeWave : public animation {
	public:
    	mergeWave()												{ min_p = 3; max_p = 20; show_time = 2; l = r = 0; index = len = 0;}
    	virtual void	init(void);
    	virtual void	show(void);
	private:
    	int				l, r;
    	uint8_t			index;
    	uint8_t			len;
};

//---------------------------------------------- Fast collide of two colors ---------------------------------------------
class collideOne : public animation {
	public:
		collideOne()											{ min_p = 1; max_p = 3; show_time = 2; cl = cr = 0; l = r = 0; boom = true; }
		virtual void	init(void);
		virtual void	show(void);
	private:
		COLOR			cl, cr;
		int16_t			l, r;
		bool			boom;
};

//---------------------------------------------- Neo fire animation by Robert Ulbricht ----------------------------------
class neoFire : public animation, public BLEND {
	public:
    	neoFire()												{ min_p = 1; max_p = 5; color = 0; pause = 0; }
    	virtual void	init(void);
    	virtual void	show(void);
	private:
    	COLOR			color;
    	int8_t			pause;
    	const 	COLOR	c1 = 80;
    	const 	COLOR	c2 = 25;
};

//---------------------------------------------- Even and odd position leds are moving towards each other ---------------
class evenOdd : public animation {
	public:
    	evenOdd()												{ min_p = 10; max_p = 20; cl = cr = 0; l = r = 0; }
    	virtual void	init(void);
    	virtual void	show(void);
	private:
    	COLOR			cl, cr;
    	int16_t			l, r;
};

//---------------------------------------------- aRandom colors from left and right move to the center -------------------
class collMdl : public animation, public BLEND {
	public:
    	collMdl()												{ min_p = 1; max_p = 6; show_time = 8; cl = cr = 0; l = r = ml = mr = 0; clr = false; }
    	virtual void	init(void);
    	virtual void	show(void);
	private:
    	void			newColors(void);
    	COLOR			cl, cr;
    	int16_t			l, r, ml, mr;
    	bool			clr;
};

//------------------------------------------- aRandom colors from left and right move to the other end -------------------
class collEnd : public animation, public BLEND {
	public:
    	collEnd()												{ min_p = 1; max_p = 6; show_time = 10; cl = cr = 0; l = r = ml = mr = 0; }
    	virtual void	init(void);
    	virtual void	show(void);
	private:
    	void			newColors(void);
    	COLOR			cl, cr;
    	int16_t			l, r, ml, mr;
};

//------------------------------------------- Rainbow colors blend --------------------------------------------------------
class rainBlend : public animation, public BLEND {
	public:
    	rainBlend()												{ min_p = 1; max_p = 6; index = 0; }
    	virtual void 	init(void)								{ index = 0; }
    	virtual void 	show(void);
	private:
    	int16_t			index;
};

//---------------------------------------------- Color swing --------------------------------------------------------------
class swing : public animation, public CRAWL {
	public:
    	swing()													{ min_p = 3; max_p = 10; show_time = 10; len = index = 0; w = 0; rnd = false; }
    	virtual void 	init(void);
    	virtual void 	show(void);
	private:
    	int16_t			len, index;
    	uint8_t			w;
    	bool			rnd;
};

//---------------------------------------------- Single color swing -------------------------------------------------------
class swingSingle : public animation {
	public:
    	swingSingle()											{ min_p = 3; max_p = 10; show_time = 10; color = 0; len = index = 0; w = 0; fwd = true; }
    	virtual void 	init(void);
    	virtual void 	show(void);
	private:
    	COLOR			color;
    	int16_t			len, index;
    	bool			fwd;
    	uint8_t			w;
};

//---------------------------------------------- Fill the strip by the color in aRandom order ----------------------------
class randomFill :  public animation, public BRGTN, public BLEND {
	public:
    	randomFill()											{ min_p = 2; max_p = 4; show_time = 15; }
    	virtual void 	init(void);
    	virtual void 	show(void);
	private:
    	void			newDot(bool clr);
    	uint8_t			w			= 0;
    	int16_t			remain		= 0;
    	uint16_t		pos			= 0;
    	bool			clr			= false;
};

//---------------------------------------------- Slow shining and flash by the different colors ---------------------------
class shineFlash :  public animation, public BRGTN {
	public:
    	shineFlash()											{ min_p = 4; max_p = 20; show_time = 12; w = mode = wait = 0; remain = indx = 0; flash = false; }
    	virtual void 	init(void);
    	virtual void 	show(void);
	private:
    	void 			startNewColor(void);
    	uint8_t			w;
    	uint8_t 		mode;
    	bool 			flash;
    	int16_t			remain;
    	int16_t			indx;
    	int8_t			wait;
};

//---------------------------------------------- Show single wave moving in Random direction ------------------------------
class singleWave : public animation, public BRGTN {
	public:
    	singleWave()											{ min_p = 4; max_p = 20; show_time = 12; pos = 0; stp = remain = incr = mode = w = 0; }
    	virtual void 	init(void);
    	virtual void 	show(void);
	private:
    	COLOR			dot[5];
    	int16_t			pos;
    	uint8_t			stp;
    	uint8_t			remain;
    	int8_t			incr;
    	uint8_t			mode;
    	uint8_t			w;
};

//---------------------------------------------- Several worms are moving Randomly ---------------------------------------
class worms : public animation, public BRGTN, public TWOCLR {
	public:
    	worms()													{ min_p = 10; max_p = 20; show_time = 9; active = 0; }
    	virtual void	init(void);
    	virtual void	show(void);
	private:
    	void			add(void);
    	void			die(uint8_t index);
    	struct worm {
    		COLOR		color;
    		int16_t		pos;
    		bool		fwd;
    	};
    	struct			worm w[5];
    	uint8_t			active;
};

//---------------------------------------------- Show interferention from many sources ----------------------------------
class interfer : public animation, public TWOCLR {
  #define num_inter 3
	public:
    	interfer()												{ min_p = 6; max_p = 10; show_time = 6; tm = active = 0; }
    	virtual void	init(void);
    	virtual void	show(void);
	private:
    	void			add(void);
    	uint32_t		clr(int p, uint8_t source);
    	int16_t			tm;										// Time the animation starts (in cycles)
    	int16_t			pos[num_inter];							// The position of the source
    	int16_t			start[num_inter];						// Time when the source activated
    	uint8_t			w[num_inter];							// Wheel Color index of the source
    	uint8_t			active;									// The number of active sources
};

//------------------------------------------- Random  colors toward each other ------------------------------------------
class toward : public animation {
	public:
    	toward()												{ min_p = 5; max_p = 10; show_time = 6; w = 0; }
    	virtual void 	init(void)								{ w = Random(256); }
    	virtual void 	show(void);
	private:
    	uint8_t			w;
};

//------------------------------------------- Raindow colors toward each other ------------------------------------------
class towardRain : public animation {
	public:
    	towardRain()											{ min_p = 10; max_p = 20; show_time = 8; w = 0; }
    	virtual void	init(void)								{ w = Random(256); }
    	virtual void	show(void);
	private:
    	uint8_t			w;
};

//---------------------------------------------- Slow single wave moving in aRandom direction -- ---------------------------
class lghtHouse : public animation, public BRGTN {
	public:
    	lghtHouse()												{ min_p = 2; max_p = 10; show_time = 6; pos = 0; stp = incr = sp = dlay  = 0; }
    	virtual void	init(void);
    	virtual void	show(void);
	private:
    	COLOR			dot[5];
    	int16_t			pos;
    	uint8_t			stp;
    	int8_t			incr;
    	uint8_t			sp;
    	uint8_t			dlay;
};

//---------------------------------------------- Random dropped color dot fading out from epicenter ----------------------
class rndDrops : public animation, public BRGTN, public TWOCLR {
	public:
    	rndDrops()												{ min_p = 2; max_p = 7; show_time = 9; num = 0; }
    	virtual void 	init(void)								{ num = 0; add(); }
    	virtual void 	show(void);
	private:
    	void 			add(void);
    	struct drop {
    		int16_t		pos;
    		int8_t		tm;
    	};
    	struct			drop dr[16];
    	int8_t			num;
};

// --------------------------------------------- Solitons are creaping up or down -----------------------------------------
class solCreep: public animation, public CRAWL {
	public:
    	solCreep(void)											{ min_p = 6; max_p = 14; show_time = 9; space = change_direction = sol = 0; w = 0; }
    	virtual void 	init(void);
    	virtual void 	show(void);
	private:
    	void			newSoliton(void);
    	int16_t			space;									// space between the solitons
    	int16_t			change_direction;						// the time to change direction
    	COLOR			dot[5];
    	int16_t			sol;
    	uint8_t			w;
};

// --------------------------------------------- Theatre-style crawling lights from NEOPIXEL example ----------------------
class theatChase: public animation {
	public:
		theatChase(void)										{ min_p = 3; max_p = 8; show_time = 3; w = color_period = color_shift = stp = 0; }
    	virtual void	init(void);
    	virtual void	show(void);
	private:
    	uint8_t			w;										// color wheel index
    	uint8_t			color_period;							// color period shift
    	int8_t			color_shift;
    	uint8_t			stp;
};

// --------------------------------------------- Meteors falling down -----------------------------------------------------
class meteorSky: public animation, public BRGTN {
	public:
		meteorSky(void)											{ min_p = 1; max_p = 3; }
		virtual void	init(void);
		virtual void	show(void);
	private:
		bool			grow	= true;
		uint16_t		head	= 0;
		uint16_t		tail	= 0;
};

// --------------------------------------------- Symmetrical dots run -----------------------------------------------------
class symmRun: public animation, public BRGTN {
	public:
		symmRun(void)											{ min_p = 0; max_p = 3; }
		virtual void	init(void);
		virtual void	show(void);
	private:
		uint16_t		left	= 0;
		uint16_t		right	= 0;
		uint8_t			w 		= 0;
		uint8_t			phase	= 0;
		uint8_t			drk_stp = 0;
};

// --------------------------------------------- Defined Meteors falling down fast ----------------------------------------
class metSingle: public animation {
	public:
		metSingle(void)											{ min_p = 1; max_p = 3; show_time = 3; }
		virtual void	init(void);
		virtual void	show(void);
	private:
		uint8_t			head	= 0;
		const COLOR		clr[8] 	= { 0xffb844, 0xfcff96, 0xfdffce, 0xffffff, 0xffb540, 0xfe9e4a, 0xe65634, 0x91182b };
		const uint8_t	clr_size = 8;
};

// --------------------------------------------- Classical multicolor strip simulation ------------------------------------
class pureStrip: public animation, public BRGTN {
	public:
		pureStrip(void)											{ }
		virtual void	init(void);
		virtual void	show(void);
	private:
		uint8_t			mode		= 0;
		uint8_t			stage		= 0;
		uint8_t			blink		= 0;
		uint8_t			num_color	= 0;						// The number of different colors in the strip 2-6
		COLOR			clr[6];									// Colors of the strip
};

// --------------------------------------------- Multiple attemption to fill up the strip from one side -------------------
class sideFill: public animation, public BRGTN {
	public:
		sideFill(void)											{ min_p = 2; max_p = 4; show_time = 4; }
		virtual void	init(void);
		virtual void	show(void);
	private:
		bool			fwd			= false;					// Which side to fill-up
		bool			on			= true;
		bool			rainbow		= false;					// Whether use a rainbow color or just monochrome
		uint16_t 		stage		= 0;						// Current maximum length of lit leds
		uint16_t		index		= 0;						// The index of led that should be lit in this step
		uint8_t			w			= 0;						// Color wheel index
};

// --------------------------------------------- Browian Motion with the tail --------------------------------------------
class browMotion: public animation, public BRGTN {
	public:
		browMotion(void)										{ min_p = 3; max_p = 8; show_time = 4; }
		virtual void	init(void);
		virtual void	show(void);
	private:
		void			newDestination(uint16_t num_pixels);
		uint8_t			w			= 0;						// color wheel index
		int8_t			speed		= 0;						// distance of one move
		uint16_t		pos			= 0;						// current position
		uint16_t		destination	= 0;						// next destination index (0 - num_pixel)
};

// --------------------------------------------- Rain drops running down -------------------------------------------------
class rainDrops: public animation, public BRGTN {
	public:
		rainDrops(void)											{ min_p = 6; max_p = 16; show_time = 4; }
		virtual void	init(void);
		virtual void	show(void);
	private:
    	struct r_drop {
    		uint16_t	head;									// Position of the drop
    		COLOR		c;										// color of the drop
    		uint8_t		speed;									// Speed: the number of pixel crossed by one move
    	};
    	void			newDrop(void);
    	const uint8_t	max_drops	= 10;
    	struct r_drop	drop[10];
    	uint8_t			active_drops = 0;
};

// --------------------------------------------- Ripe fruits booms and run two pieces ------------------------------------
class ripeFruit: public animation, public BRGTN {
	public:
		ripeFruit(void)											{ min_p = 6; max_p = 16; show_time = 4; }
		virtual void	init(void);
		virtual void	show(void);
	private:
    	struct r_fruit {
    		uint16_t	part[2];								// Position of the half of the fruit
    		COLOR		c;										// color of the fruit
    		uint8_t		speed;									// Speed: the number of pixel crossed by one move
    	};
    	void			newFruit(void);
    	const uint8_t	max_fruits	= 10;
    	struct r_fruit	fruit[10];
    	uint8_t			active_fruits = 0;
};

// --------------------------------------------- The single color that changes the brightness as a sine ------------------
class brightWave: public animation, public WAVE {
	public:
		brightWave(void)										{ min_p = 6; max_p = 16; show_time = 4; }
		virtual void	init(void);
		virtual void	show(void);
	private:
		uint8_t			w	= 0;								// The color wheel index
		uint8_t			t	= 0;
		bool			fwd	= false;
};

// --------------------------------------------- Color pixels walking up and down with sine brightness -------------------
class brColCreep: public animation, public WAVE, public CRAWL {
	public:
		brColCreep(void)										{ min_p = 6; max_p = 16; show_time = 4; }
		virtual void	init(void);
		virtual void	show(void);
	private:
		uint8_t			w	= 0;								// The color wheel index
		uint8_t			t	= 0;
};

#endif
