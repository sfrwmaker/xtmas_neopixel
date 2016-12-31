#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

const byte NEO_BRGHT    = 255;
const byte NEO_PIN      = 6;                              // Pin of Neopixel Strip
const byte StripSize    = 100;                            // Length of Neopixel Strip
const byte min_time     = 30;                             // Minimal sequence show time (secons)

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(StripSize, NEO_PIN, NEO_GRB + NEO_KHZ800);

//---------------------------------------------- Base animatio class with usefull functions -------------------------------
class BASE {
  public:
    uint32_t Wheel(byte WheelPos);
};

// Input a value 0 to 255 to get a color value. The colours are a transition r - g - b - back to r.
uint32_t BASE::Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

//---------------------------------------------- Classes for strip animation ----------------------------------------------
class animation : public BASE {
  public:
    animation() { min_p = 2; max_p = 8; show_time = min_time / 10; do_clear = false; complete = true; }
    virtual  void init(void) = 0;
    virtual  void show(void) = 0;
	  void     get_limits(byte& mi_p, byte& ma_p)           { mi_p = min_p; ma_p = max_p; }
    byte     min_p;                                       // The minimum period in tenth of second to show the stage
	  byte     max_p;                                       // The maximum period in tenth of second to show the stage
    byte     show_time;                                   // The minimum time to show the sequence in 10-secons intervals
    bool     do_clear;                                    // Whether the strip have to be cleaned for the next loop
    bool     complete;                                    // Whether the animation can be changed to the next one
};

// --------------------------------------------- creep the sequence up or down, superclass --------------------------------
class CRAWL {
  public:
    CRAWL(void)                                           { fwd = true; }
    void step(void);
  protected:
    bool fwd;                                             // direction to crawl: false - backward, true - formard
    uint32_t next_color;
};

void CRAWL::step(void) {
  if (fwd) {                                              // creep forward
    for (int i = strip.numPixels()-1; i > 0; --i) {
      uint32_t c = strip.getPixelColor(i-1);
      strip.setPixelColor(i, c);
    }
  strip.setPixelColor(0, next_color);
  } else {                                                // creep backward
    int last = strip.numPixels()-1;
    for (int i = 0; i < last; ++i) {
      uint32_t c = strip.getPixelColor(i+1);
      strip.setPixelColor(i, c);
    }
    strip.setPixelColor(last, next_color);
  }
}

//---------------------------------------------- Brighttess manipulation --------------------------------------------------
class BRGTN {
  public:
    BRGTN()                                               { }
    void setColor(uint32_t c);
    bool change(uint16_t index, int val);
    bool changeAll(int val);
  protected:
    byte color[3];
};

void BRGTN::setColor(uint32_t c) {
   color[0] = c & 0xff;
   c >>= 8;
   color[1] = c & 0xff;
   c >>= 8;
   color[2] = c & 0xff;
}

bool BRGTN::changeAll(int val) {
  bool finish = true;
  for (byte i = 0; i < strip.numPixels(); ++i) {
    if (!change(i, val)) finish = false;
  }
  return finish;
}

bool BRGTN::change(uint16_t index, int val) {
  uint32_t c = strip.getPixelColor(index);
  byte bound = 0;
  int  e = 256 + val;
  for (char s = 2; s >= 0; --s) {
    long cc = c >> (s*8);
    cc &= 0xff;
    long cs = cc;
    cc *= e; cc >>= 8;
    if (cs == cc) cc += val;
    if ((val > 0) && (cc >= color[byte(s)])) {
      cc = color[byte(s)];
      bound ++;
    }
    if ((val < 0) && (cc <= 0)){
      cc = 0;
      bound ++;
    }
    uint32_t mask = 0xff; mask <<= (8*s); mask = ~mask;
    cc <<= (s*8);
    c &= mask;
    c |= cc;
  }
  strip.setPixelColor(index, c);
  return (bound >= 3);
}

//---------------------------------------------- Blend manipulations ------------------------------------------------------
class BLEND {
  public:
    BLEND()                                               {}
    uint32_t add(uint32_t color1, uint32_t color2);
    uint32_t sub(uint32_t color1, uint32_t color2);
    void     blendPixel(uint16_t p);
};

uint32_t BLEND::add(uint32_t color1, uint32_t color2) {
  uint8_t r1,g1,b1;
  uint8_t r2,g2,b2;

  r1 = (uint8_t)(color1 >> 16),
  g1 = (uint8_t)(color1 >>  8),
  b1 = (uint8_t)(color1 >>  0);

  r2 = (uint8_t)(color2 >> 16),
  g2 = (uint8_t)(color2 >>  8),
  b2 = (uint8_t)(color2 >>  0);

  return strip.Color(constrain(r1+r2, 0, 255), constrain(g1+g2, 0, 255), constrain(b1+b2, 0, 255));
}

uint32_t BLEND::sub(uint32_t color1, uint32_t color2) {
  uint8_t r1,g1,b1;
  uint8_t r2,g2,b2;
  int16_t r,g,b;

  r1 = (uint8_t)(color1 >> 16),
  g1 = (uint8_t)(color1 >>  8),
  b1 = (uint8_t)(color1 >>  0);

  r2 = (uint8_t)(color2 >> 16),
  g2 = (uint8_t)(color2 >>  8),
  b2 = (uint8_t)(color2 >>  0);

  r  = (int16_t)r1 - (int16_t)r2;
  g  = (int16_t)g1 - (int16_t)g2;
  b =  (int16_t)b1 - (int16_t)b2;
  if (r < 0) r = 0;
  if (g < 0) g = 0;
  if (b < 0) b = 0;

  return strip.Color(r, g, b);
}

void BLEND::blendPixel(uint16_t p) {
  byte r1 = random(20);
  byte g1 = random(20);
  byte b1 = random(20);
  uint32_t diff_color = strip.Color(r1, g1, b1);
  uint32_t blended_color = BLEND::add(diff_color, strip.getPixelColor(p));
  r1 = random(20);
  g1 = random(20);
  b1 = random(20);
  diff_color = strip.Color(r1, g1, b1);
  blended_color = BLEND::sub(blended_color, diff_color);
  strip.setPixelColor(p, blended_color);
}

//---------------------------------------------- Color superposition class ------------------------------------------------
class TWOCLR {
  public:
    TWOCLR()                                              {}
    uint32_t add(uint32_t c1, uint32_t c2);
};

uint32_t TWOCLR::add(uint32_t c1, uint32_t c2) {
  uint32_t c = 0;
  for (byte i = 0; i < 3; ++i) {
    uint32_t p = c1 >> (i*8);
    p += c2 >> (i*8);
    p &= 0xff;
    p <<= i*8;
    c |= p;
  }
  return c;
}

// --------------------------------------------- Fill the dots one after the other with a color ---------------------------
class colorWipe: public animation {
  public:
    colorWipe(void)                                       { w = random(256); }
    virtual void init(void);
    virtual void show(void);
  private:
    byte  w;
    int   index;
    bool  fwd;
};

void colorWipe::init(void) {
  int p = random(2, 4);
  w  += p*16 + 1;
  fwd = random(2);
  index = 0;
  if (!fwd) index = strip.numPixels() - 1;
}

void colorWipe::show(void) {
  uint32_t color = Wheel(w);
  if (fwd) {
    if (index > int(strip.numPixels())) {                 // Start new sequence with the new color
      init();
      complete = true;
      return;
    }
    strip.setPixelColor(index++, color);
  } else {
    if (index < 0) {                                      // Start new sequence with the new color
      init();
      complete = true;
      return;
    }
    strip.setPixelColor(index--, color);
  }
  complete = false;
}

// --------------------------------------------- Walk the dots one after the other with a color ---------------------------
class colorWalk: public animation {
  public:
    colorWalk(void)                                       { min_p = 4; max_p = 12;}
    virtual void init(void);
    virtual void show(void);
  private:
    int   index;
    byte  period;
    bool  fwd;
    byte  w;
};

void colorWalk::init(void) {
  index  = 0;
  w      = random(256);
  fwd    = random(2);
  period = random(10, 30);
}

void colorWalk::show(void) {
  int n = strip.numPixels();
  if (fwd) {
    if (index > n) {
      index -= period;
      strip.setPixelColor(n-1, 0);
    }
    uint32_t color = Wheel(w--);
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
    uint32_t color = Wheel(w++);
    for (int i = index; i < n; i += period) {
      if (i < int(strip.numPixels() - 1)) strip.setPixelColor(i+1, 0);
      strip.setPixelColor(i, color);
    }
    --index;
  }
}

// --------------------------------------------- creep the random sequence up or down -------------------------------------
class randomCreep: public CRAWL, public animation {
  public:
    randomCreep(void)                                     { min_p = 6; max_p = 24;}
  	virtual void init(void);
    virtual void show(void);
  private:
    byte space;                                           // space between the color dots
	  int  change_direction;
	  byte cnt;
};

void randomCreep::init(void) {
  space = random(2, 5);
  change_direction = random(100, 500);
  cnt = 0;
}

void randomCreep::show(void) {
  -- change_direction;
  if (change_direction <= 0) {
    CRAWL::fwd = !CRAWL::fwd;
    init();
  }
	
  next_color = 0;
  ++cnt;
  if (cnt > space) {
    cnt = 0;
	next_color = Wheel(random(256));
  }

  CRAWL::step();
}

// --------------------------------------------- show the color Wave using rainbowCycle -----------------------------------
class colorWave: public CRAWL, public BRGTN, public animation {
  public:
    colorWave(void)                                       { }
    virtual void init(void);
    virtual void show(void);
  private:
    byte index;
    bool rdy;
};

void colorWave::init(void) {
  index = 0;
  rdy = false;
  CRAWL::fwd = random(2);
}

void colorWave::show(void) {
  if (!rdy) {
    rdy = true;
    for(uint16_t i = 0; i < strip.numPixels(); ++i) {
      BRGTN::setColor(Wheel(((i * 256 / strip.numPixels())) & 255));
      if (!BRGTN::change(i, 2)) rdy = false;
    }
    return;
  }

  CRAWL::step();
  if (CRAWL::fwd)
    strip.setPixelColor(0, Wheel(index & 255));
  else
    strip.setPixelColor(strip.numPixels() - 1, Wheel(index & 255));
  ++index;
}

// --------------------------------------------- show the rainbow (from neopixel example) ---------------------------------
class rainbow: public BRGTN, public animation {
  public:
    rainbow(void)                                         {}
    virtual void init(void)                               { index = 0; rdy = false; }
    virtual void show(void);
  private:
    byte index;
    bool rdy;
};

void rainbow::show(void) {
  if (!rdy) {
    rdy = true;
    for(uint16_t i = 0; i < strip.numPixels(); ++i) {
      BRGTN::setColor(Wheel(i & 255));
      if (!BRGTN::change(i, 2)) rdy = false;
    }
    return;
  }

  for(uint16_t i = 0; i < strip.numPixels(); ++i) {
    strip.setPixelColor(i, Wheel((i+index) & 255));
  }
  ++index;                                                // index is from 0 to 255
}

// --------------------------------------------- show the rainbowCycle (from neopixel example) ----------------------------
class rainCycle: public BRGTN, public animation {
  public:
    rainCycle(void)                                       {}
    virtual void init(void)                               { index = 0; rdy = false; }
    virtual void show(void);
  private:
    byte index;
    bool rdy;
};

void rainCycle::show(void) {
  if (!rdy) {
    rdy = true;
    for(uint16_t i = 0; i < strip.numPixels(); ++i) {
      BRGTN::setColor(Wheel((i * 256 / strip.numPixels()) & 255));
      if (!BRGTN::change(i, 1)) rdy = false;
    }
    return;
  }

  for(uint16_t i = 0; i < strip.numPixels(); ++i) {
    strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + index) & 255));
  }
  ++index;                                                // index is from 0 to 255
}

// --------------------------------------------- show the rainbow colors on the entire strip ------------------------------
class rainFull: public BRGTN, public animation {
  public:
    rainFull(void)                                        { min_p = 6; max_p = 24; }
    virtual void init(void)                               { index = random(256); rdy = false; }
    virtual void show(void);
  private:
    byte index;
    bool rdy;
};

void rainFull::show(void) {
  if (!rdy) {
    rdy = true;
    for(uint16_t i = 0; i < strip.numPixels(); ++i) {
      BRGTN::setColor(Wheel(index));
      if (!BRGTN::change(i, 1)) rdy = false;
    }
    return;
  }

  for(uint16_t i = 0; i < strip.numPixels(); ++i) {
    strip.setPixelColor(i, Wheel(index));
  }
  ++index;                                                // index is from 0 to 255
}

//---------------------------------------------- Light up with the random color than fade out -----------------------------
class lightUp : public BRGTN, public animation {
  public:
    lightUp()                                             { min_p = 6; max_p = 24; show_time = 12; }
    virtual void init(void)                               { sp = 1; inc = sp; newColor(); }
    virtual void show(void);
  private:
    void newColor(void);
    byte sp;
    char inc;
};

void lightUp::show(void) {
  bool finish = BRGTN::changeAll(inc);
  if (finish) {
    if (inc > 0)
      inc = -8;
    else {
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
  for (byte i = 0; i < 3; ++i) {
    c <<= 8;
    byte d = random(16) << 4;
    c |= d;
  }
  BRGTN::setColor(c);
}

//---------------------------------------------- Random sparcs ------------------------------------------------------------
class sparks : public BRGTN, public animation {
  public:
    sparks()                                              { min_p = 2; max_p = 6; show_time = 12; }
    virtual void init(void)                               { for (byte i = 0; i < 8; ++i) pos[i] = 0; }
    virtual void show(void);
  private:
    uint16_t pos[8];
};

void sparks::show(void) {
  uint32_t c = Wheel(random(265));
  for (char i = 7; i >= 1; --i) {
    if (i == 6)
      strip.setPixelColor(pos[byte(i)], 0);
    else
      BRGTN::change(pos[byte(i)], -128);
    pos[byte(i)] = pos[byte(i-1)];
  }
  int p = random(strip.numPixels()+1);
  pos[0] = p;
  strip.setPixelColor(p, c);
}

//---------------------------------------------- Random sparks fade out ---------------------------------------------------
class rndFade : public BRGTN, public animation {
  public:
    rndFade()                                             { min_p = 6; max_p = 24; show_time = 12; }
    virtual void init(void)                               {}
    virtual void show(void);
};

void rndFade::show(void) { 
  BRGTN::changeAll(-16);
  byte newDot = random(1, 5);
  for (byte i = 0; i < newDot; ++i) {
    int     p = random(strip.numPixels()+1);
    uint32_t c = Wheel(random(256));
    if (strip.getPixelColor(p) == 0)
      strip.setPixelColor(p, c);
  }
}

//---------------------------------------------- Lights run from the center -----------------------------------------------
class centerRun : public animation {
  public:
    centerRun()                                           { min_p = 4; max_p = 20; }
    virtual void init(void);
    virtual void show(void);
  private:
    uint32_t color;
    int     m, l, r;
};

void centerRun::init(void) {
  color = Wheel(random(256));
  int n    = strip.numPixels();
  int diff = n >> 3;
  m = l = r = random(diff+2) + ((n * 7) >> 4);
}

void centerRun::show(void) {
  if (l >= 0) strip.setPixelColor(l, color);
  if (r < int(strip.numPixels())) strip.setPixelColor(r, color);
  l -= 2; r += 2;
  if ((l < 0) && r >= int(strip.numPixels())) {
    l = r = m + 1;
    color = Wheel(random(256));
  }
}

//---------------------------------------------- Slow shining by the different colors -------------------------------------
class shineSeven :  public BRGTN, public animation {
  public:
    shineSeven()                                          { min_p = 4; max_p = 20; show_time = 12; }
    virtual void init(void);
    virtual void show(void);
  private:
    void startNewColor(void);
    byte curs;
    byte w;
    byte base;
};

void shineSeven::init(void) {
  curs = 0;
  w    = random(256);
  base = random(4, 8);
  startNewColor();
}

void shineSeven::show() {
  int n = strip.numPixels();
  bool finish = true;
  for (int i = int(curs) - 1; i < n; i += base) {         // Fade out previous color
    if (i >= 0)
      if(!BRGTN::change(i, -8)) finish = false;
  }
  for (int i = curs; i < n; i += base)                    // Light up current color
    if (!BRGTN::change(i, 8)) finish = false;
  if (finish) {                                           // The current color has been light fully
    ++curs; if (curs >= base) curs = 0;
    startNewColor();
  }
}

void shineSeven::startNewColor(void) {
  uint32_t c = Wheel(w);
  w += 97;
  BRGTN::setColor(c);
  c &= 0x10101;
  for (uint16_t i = curs; i < strip.numPixels(); i += base)
    strip.setPixelColor(i, c);
}

//---------------------------------------------- Rapid walking by the different colors -----------------------------------
class walkSeven : public BRGTN, public TWOCLR, public animation {
  public:
    walkSeven()                                           { min_p = 8; max_p = 15; show_time = 9; }
    virtual void init(void);
    virtual void show(void);
  private:
    char curs;
    byte w;
    bool fwd;
    int  ch_dir;
    byte period;
};

void walkSeven::init() {
  w   = random(256);
  fwd = random(2);
  ch_dir = random(30, 100);
  period = random(13, 20);
  curs = 0;
}

void walkSeven::show(void) {
  BRGTN::changeAll(-64);

  int n = strip.numPixels();
  uint32_t c1 = Wheel(w);
  w += 71;
  for (int i = curs; i < n; i += period) {
     uint32_t c2 = strip.getPixelColor(i);
     c2 = TWOCLR::add(c1, c2);
     strip.setPixelColor(i, c2);
  }

  if (fwd) ++curs; else --curs;
  curs %= period;

  if (--ch_dir < 0) {
    ch_dir = random(70, 300);
    fwd = !fwd;
  }
}

//---------------------------------------------- Rapid flashing by the differenc colors ---------------------------------
class flashSeven : public BRGTN, public animation {
  public:
    flashSeven()                                          { min_p = 4; max_p = 8; show_time = 9; }
    virtual void init(void);
    virtual void show(void);
  private:
    char curs;
    byte w;
    bool fwd;
    int  ch_dir;
    byte period;
};

void flashSeven::init() {
  w   = random(256);
  fwd = random(2);
  ch_dir = random(30, 100);
  period = random(7, 20);
  curs = 0;
}

void flashSeven::show(void) {
  BRGTN::changeAll(-64);

  int n = strip.numPixels();
  for (int i = curs; i < n; i += period) {
     BRGTN::change(i, -128);
  }
  
  if (fwd) ++curs; else --curs;
  curs %= period;

  uint32_t c = Wheel(w);
  w += 71;
  for (int i = curs; i < n; i += period) {
     strip.setPixelColor(i, c);
  }

  if (--ch_dir < 0) {
    ch_dir = random(70, 300);
    fwd = !fwd;
  }
}

//---------------------------------------------- Slow merging of two colors -----------------------------------------------
class mergeOne : public TWOCLR, public animation {
  public:
    mergeOne()                                            { min_p = 8; max_p = 30; }
    virtual void init(void);
    virtual void show(void);
  private:
    uint32_t cl, cr;
    int      l, r;
};

void mergeOne::init(void) {
  l = 0; 
  r = strip.numPixels();
  byte indx = random(256);
  cl = Wheel(indx);
  indx += random(4, 16);
  cr = Wheel(indx);
  strip.clear();
}

void mergeOne::show(void) {
  if (l < r) {
    strip.setPixelColor(l, cl);
    strip.setPixelColor(r, cr);
  } else {
    uint32_t c = strip.getPixelColor(l);
    c = TWOCLR::add(c, cl);
    strip.setPixelColor(l, c);
    c = strip.getPixelColor(r);
    c = TWOCLR::add(c, cr);
    strip.setPixelColor(r, c);
  }

  --r; ++l;
  if (r < 0) {                                            // Force the strip clerance
    do_clear = true;
    complete = true;
    return;
  }
  complete = false;
}

//---------------------------------------------- Fast merging of Waves ----------------------------------------------------
class mergeWave : public animation {
  public:
    mergeWave()                                           { min_p = 3; max_p = 20; show_time = 2; }
    virtual void init(void);
    virtual void show(void);
  private:
    int  l, r;
    byte index;
    byte len;
};

void mergeWave::init(void) {
  l = 0; 
  r = strip.numPixels() - 1;
  index = random(256);
  len   = random(8, 17); 
  strip.clear();
}

void mergeWave::show(void) {
  if (l < r) {
    strip.setPixelColor(l, Wheel(l & 255));
    if (l > len) strip.setPixelColor(l-len, 0);
    strip.setPixelColor(r, Wheel((index + r) & 255));
    if ((r + len) > int(strip.numPixels())) strip.setPixelColor(r+len, 0);
  } else {
    uint32_t c = strip.getPixelColor(l);
    c |= Wheel(l & 255);
    strip.setPixelColor(l, c);
    c = strip.getPixelColor(r);
    c |= Wheel((index + r) & 255);
    strip.setPixelColor(r, c);
    strip.setPixelColor(l-len, 0);
    strip.setPixelColor(r+len, 0);
  }
  --r; ++l; index += 4;
  if (r < 0) {                                            // Force the strip clerance
    do_clear = true;
    complete = true;
  }
  complete = false;
}

//---------------------------------------------- Fast collide of two colors ---------------------------------------------
class collideOne : public animation {
  public:
    collideOne()                                          { min_p = 1; max_p = 3; show_time = 2; }
    virtual void init(void);
    virtual void show(void);
  private:
    uint32_t cl, cr;
    int      l, r;
    bool     boom;
};

void collideOne::init(void) {
  l = 0; 
  r = strip.numPixels() - 1;
  byte w = random(256);
  cl = Wheel(w);
  w += random(8, 16);
  cr = Wheel(w);
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
class neoFire : public BLEND, public animation {
  public:
    neoFire()                                             { min_p = 1; max_p = 5; }
    virtual void init(void);
    virtual void show(void);
  private:
    uint32_t color;
    char     pause;
    const uint32_t c1 = 80;
    const uint32_t c2 = 25;
};

void neoFire::init(void) {
  color = 0;
  byte m = random(3);
  color |= c1 << (m*8);
  m += random(1,3);
  if (m >= 3) m = 0;
  color |= c2 << (m*8);
}

void neoFire::show(void) {
  if (pause > 0) {
    --pause;
    return;
  }
  pause = random(8);
  for(uint16_t i = 0; i < strip.numPixels(); ++i) {
    uint32_t blended_color = BLEND::add(strip.getPixelColor(i), color);
    byte r = random(80);
    uint32_t diff_color = strip.Color(r, r/2, r/2);
    blended_color = BLEND::sub(blended_color, diff_color);
    strip.setPixelColor(i, blended_color);
  }
}

//---------------------------------------------- Even and odd position leds are moving towards each other ---------------
class evenOdd : public animation {
  public:
    evenOdd()                                             { min_p = 10; max_p = 20; }
    virtual void init(void);
    virtual void show(void);
  private:
    uint32_t cl, cr;
    int      l, r;
};

void evenOdd::init(void) {
  byte indx = random(256);
  cl = Wheel(indx);
  indx += random(4, 16);
  cr = Wheel(indx);
  l = 0;
  r = strip.numPixels() - 1;                              // r is Odd
}

void evenOdd::show(void) {
  strip.setPixelColor(l, cl);
  strip.setPixelColor(r, cr);
  l += 2; r -= 2;
  if (r < 0) {
    if (cr) {
      cr = cl = 0;
      l = 0;
      r = strip.numPixels() - 1;                          // r is Odd
    } else {
      init();
      complete = true;
      return;
    }
  }
  complete = false;
}

//---------------------------------------------- Random colors from left and right move to the center -------------------
class collMdl : public BLEND, public animation {
  public:
    collMdl()                                             { min_p = 1; max_p = 6; show_time = 8; }
    virtual void init(void);
    virtual void show(void);
  private:
    void     newColors(void);
    uint32_t cl, cr;
    int      l, r, ml, mr;
    bool     clr;
};

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
      BLEND::blendPixel(i);
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
  cl = Wheel(random(256));
  cr = Wheel(random(256));
  l = 0;
  r = strip.numPixels() - 1;
}

//------------------------------------------- Random colors from left and right move to the other end -------------------
class collEnd : public BLEND, public animation {
  public:
    collEnd()                                             { min_p = 1; max_p = 6; show_time = 10; }
    virtual void init(void);
    virtual void show(void);
  private:
    void     newColors(void);
    uint32_t cl, cr;
    int      l, r, ml, mr;
};

void collEnd::init(void) {
  ml = strip.numPixels() - 1;
  mr = 0;
  newColors();
}

void collEnd::show(void) {
  // blend colors in the both ends
  if (mr > 1) {
    for (int i = 0; i < mr; ++i)
      BLEND::blendPixel(i);
    for (uint16_t i = ml; i < strip.numPixels(); ++i)
      BLEND::blendPixel(i);
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
      do_clear = true;                                    // Force the strip clerance
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
  cl = Wheel(random(256));
  cr = Wheel(random(256));
  l = mr;
  r = ml;
}

//------------------------------------------- Rainbow colors blend --------------------------------------------------------
class rainBlend : public BLEND, public animation {
  public:
    rainBlend()                                           { min_p = 1; max_p = 6; }
    virtual void init(void)                               { index = 0; }
    virtual void show(void);
  private:
    int index;
};


void rainBlend::show(void) {
  if (index < int(strip.numPixels())) {
    strip.setPixelColor(index, Wheel(((index * 256 / strip.numPixels())) & 255));
    ++index;
    return;
  }
  for(uint16_t i = 0; i < strip.numPixels(); ++i)
    BLEND::blendPixel(i);
}

//---------------------------------------------- Color swing --------------------------------------------------------------
class swing : public CRAWL, public animation {
  public:
    swing()                                               { min_p = 3; max_p = 10; show_time = 10; }
    virtual void init(void);
    virtual void show(void);
  private:
    int  len, index;
    byte w;
    bool rnd;
};

void swing::init(void) {
  len = 1;
  rnd = random(2);
  uint32_t c = Wheel(random(256));
  if (!rnd) {                                               // Use rainbow colors from Wheel
    w = random(256);
    c = Wheel(w);
  }
  strip.setPixelColor(0, c);
  CRAWL::fwd = true;
  CRAWL::next_color = 0;
  index = strip.numPixels() - len - 1;
}

void swing::show(void) {
  CRAWL::step();
  --index;

  if (index < 0) {
    uint32_t c = Wheel(random(256));
    if (!rnd) {
      w += 4;
      c = Wheel(w);
    }
    if (CRAWL::fwd)
      strip.setPixelColor(strip.numPixels() - len - 1, c);
    else
      strip.setPixelColor(len, c);
    ++len;
    CRAWL::fwd = !CRAWL::fwd;
    index = strip.numPixels() - len - 1;
    if (len >= int(strip.numPixels())) {
      do_clear = true;                                    // Force the strip clerance
      complete = true;
      return;
    }
  }
  complete = false;
}

//---------------------------------------------- Single color swing -------------------------------------------------------
class swingSingle : public animation {
  public:
    swingSingle()                                         { min_p = 3; max_p = 10; show_time = 10; }
    virtual void init(void);
    virtual void show(void);
  private:
    uint32_t color;
    int      len, index;
    bool     fwd;
    byte     w;
};

void swingSingle::init(void) {
  len = 1;
  w = random(256);
  color = Wheel(w);
  strip.setPixelColor(0, color);
  fwd = true;
  index = len;
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
      len += random(1, strip.numPixels() >> 4);
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
    do_clear = true;                                      // Force the strip clerance
    complete = true;
    return;
  }
  complete = false;
}

//---------------------------------------------- Fill the strip by the color in random order ----------------------------
class randomFill :  public BRGTN, public animation {
  public:
    randomFill()                                          { min_p = 2; max_p = 4; show_time = 15; }
    virtual void init(void);
    virtual void show(void);
  private:
    void      newDot(bool clr);
    byte      w;
    int       remain;
    uint16_t  pos;
    bool      clr;
};

void randomFill::init(void) {
  w  = random(256);
  remain = strip.numPixels();
  clr = false;
  newDot(clr);
}

void randomFill::show() {
  char val = 12;
  if (clr) val = -12;
  if (BRGTN::change(pos, val)) {

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
  complete = false;
}

void randomFill::newDot(bool clr) {
  uint32_t color = 0;
  if (!clr) color = Wheel(w);

  byte p = random(remain);
  byte c = 0;
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
  if (pos >= strip.numPixels()) {                         // something is wrong in the code
    for (uint16_t i = 0; i < strip.numPixels(); ++i)
      strip.setPixelColor(i, color);
    remain = 0;
  }
  BRGTN::setColor(color);
  remain--;
}

//---------------------------------------------- Slow shining and flash by the different colors ---------------------------
class shineFlash :  public BRGTN, public animation {
  public:
    shineFlash()                                          { min_p = 4; max_p = 20; show_time = 12; }
    virtual void init(void);
    virtual void show(void);
  private:
    void startNewColor(void);
    byte w;
    byte mode;
    bool flash;
    int  remain;
    int  indx;
    char wait;
};

void shineFlash::init(void) {
  w    = random(256);
  mode = 0;
  flash = false;
  startNewColor();
}

void shineFlash::show() {
  int n = strip.numPixels();
  bool finish = true;
  switch(mode) {
    case 0:                                               // Light up
      finish = BRGTN::changeAll(4);
      if (finish) {
        flash  = true;
        remain = random(17, 30);
        wait   = 0;
      }
      break;
    case 1:                                               // Run flash
      if (flash) {                                        // Lit the LED
        if (--wait > 0) return;
        wait = random(2, 7);
        indx = random(n);
        if (remain > 0) {
          uint32_t c = strip.getPixelColor(indx);
          c |= 0x808080;
          strip.setPixelColor(indx, c);
          flash = false;
          remain --;
        } else {
          mode ++;
        }
      } else {
        uint32_t c = strip.getPixelColor(indx);
        c &= 0x7f7f7f;
        strip.setPixelColor(indx, c);
        flash = true;
      }
      finish = false;
      break;
    case 2:                                               // Fade out
    default:
      finish = BRGTN::changeAll(-4);
      break;
  }

  if (finish) {                                           // The current color has been light fully
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
  uint32_t c = Wheel(w);
  c &= 0x7f7f7f;
  w += 17;
  BRGTN::setColor(c);
  c &= 0x10101;
  int  n = strip.numPixels();
  for (int i = 0; i < n; ++i)
    strip.setPixelColor(i, c);
}

//---------------------------------------------- Show single wave moving in random direction -- ---------------------------
class singleWave : public BRGTN, public animation {
  public:
    singleWave()                                          { min_p = 4; max_p = 20; show_time = 12; }
    virtual void init(void);
    virtual void show(void);
  private:
    uint32_t dot[5];
    int      pos;
    byte     stp;
    byte     remain;
    char     incr;
    byte     mode;
    byte     w;
};

void singleWave::init(void) {
  uint32_t c = Wheel(random(256));
  uint32_t r = c & 0xff;
  uint32_t g = (c >> 8)  & 0xff;
  uint32_t b = (c >> 16) & 0xff;
  for (byte i = 1; i <= 4; ++i) {
    r >>= 1;
    g >>= 1;
    b >>= 1;
    uint32_t cc = b & 0xff; cc <<= 8;
    cc |= g & 0xff; cc <<= 8;
    cc |= r & 0xff;
    dot[i] = cc;
  }
  
  BRGTN::setColor(dot[3]);
  c &= 0x10101;
  int n = strip.numPixels();
  for (int i = 0; i < n; ++i)
    strip.setPixelColor(i, c);
  mode   = 0;
  pos    = random(n);
  remain = random(5, 15);
  stp    = 0;
}

void singleWave::show() {
  int n = strip.numPixels();
  bool finish = true;
  switch(mode) {
    case 0:                                               // Light up
      finish = BRGTN::changeAll(4);
      break;
    case 1:                                               // move the soliton
      finish = false;
      if (stp <= 0) {
        incr = 1;
        if (pos > n / 2) incr = -1;
        int m = n - pos - 2;
        if (incr < 0) m = pos - 2;
        stp = random(5, m);
        --remain;
        if (remain <= 0) {
          for (int i = 0; i < n; ++i)
            strip.setPixelColor(i, dot[3]);
          finish = true;
          break;
        }
      }
      pos += incr;
      for (int i = 0; i < n; ++i) {
        strip.setPixelColor(i, dot[3]);
      }
      BRGTN::changeAll(random(9) - 4);
      for (int i = 3; i > 0; --i) {
        if ((pos - i) >= 0) strip.setPixelColor(pos - i, dot[i]);
      }
      for (int i = 0; i <= 3; ++i) {
        if ((pos + i) < n) strip.setPixelColor(pos + i, dot[i]);
      }
      stp --;
      break;
    case 2:                                               // Fade out
    default:
      finish = BRGTN::changeAll(-4);
      break;
  }

  if (finish) {                                           // The current color has been light fully
    ++mode;
    if (mode >= 3) {
      init ();
      complete = true;
      return;
    }
  }
  complete = false;
}

//---------------------------------------------- Several worms are moving randomly ---------------------------------------
class worms : public BRGTN, public TWOCLR, public animation {
  public:
    worms()                                               { min_p = 10; max_p = 20; show_time = 9; }
    virtual void init(void);
    virtual void show(void);
  private:
    void add(void);
    void die(byte index);
    struct worm {
      uint32_t color;
      int      pos;
      bool     fwd;
    };
    struct worm w[5];
    byte   active;
};

void worms::init(void) {
  active   = 0;
  add();
}

void worms::show(void) {
  int n = strip.numPixels();

  // fade away
  BRGTN::changeAll(-32);

  // Move existing
  for (byte wi = 0; wi < active; ++wi) {
    int np = w[wi].pos - 1;
    if (w[wi].fwd) np += 2;
    if ((np < 0) || (np >= n)) {
      die(wi);
      --wi;
      continue;
    }
    uint32_t c = strip.getPixelColor(np);
    if ((c != 0) && (random(10) == 0)) {
      die(wi);
      --wi;
      continue;
    } else {
      c = TWOCLR::add(c, w[wi].color);
      w[wi].pos = np;
      strip.setPixelColor(np, c);
    }
  }

  if (random(12) == 0) add();
}

void worms::add(void) {
  if (active >= 5) return;

  byte mode = random(3);
  int n = strip.numPixels();
  switch (mode) {
    case 0:                                               // Run from the start
      w[active].pos = 0;
      break;
    case 1:                                               // Run from the end
      w[active].pos = n-1;
      break;
    case 2:                                               // Run from the random position
    default:
      w[active].pos = random(n);
      break;
  }
  w[active].color = Wheel(random(256));
  if (strip.getPixelColor(w[active].pos) != 0) return;
  if (w[active].pos < n/3) {
    w[active].fwd = true;
  } else if ((n - w[active].pos) < n/3) {
    w[active].fwd = false; 
  } else {
    w[active].fwd = random(2);
  }
  ++active;
}

void worms::die(byte index) {
  --active;
  w[index].color = w[active].color;
  w[index].pos   = w[active].pos;
  w[index].fwd   = w[active].fwd;
}

//---------------------------------------------- Show interferention from many sources ----------------------------------
class interfer : public TWOCLR, public animation {
  #define num_inter 3
  public:
    interfer()                                            { min_p = 6; max_p = 10; show_time = 6; }
    virtual void init(void);
    virtual void show(void);
  private:
    void     add(void);
	  uint32_t clr(int p, byte source);
    int      tm;                                          // Time the animation starts (in cycles)
	  int      pos[num_inter];                              // The position os the source
	  int      start[num_inter];                            // Time whan the source activated
	  byte     w[num_inter];                                // Wheel Color index of the source
	  byte     active;                                      // The number of active sources
};

void interfer::init(void) {
  active = 0;
  tm     = 0;
  add();
}

void interfer::show(void) {
  int n = strip.numPixels();

  for (int i = 0; i < n; ++i) {
    uint32_t c = 0;
    for (byte j = 0; j < active; ++j) {
	    uint32_t c1 = clr(i, j);
	    c = TWOCLR::add(c, c1);
	  }
	  strip.setPixelColor(i, c);
  }
  ++tm;
  if (tm %64 == 0) {
    for (byte i = 0; i < active; ++i)
      w[i] += 4;
  }
  
  if (!random(20)) add();
}

void interfer::add(void) {
  if (active >= num_inter) return;
  pos[active]   = random(strip.numPixels());
  w[active]     = random(256);
  start[active] = tm;
  active++;
}

uint32_t interfer::clr(int p, byte source) {
  uint32_t c = 0;
  int s_pos = pos[source];
  int e  = tm - start[source];
  e -= abs(p - s_pos);
  if (e < 0) return c;                                    // The wave is not here yet
  e %= 64;                                                // The wave period
  byte elm = 0;
  if (e < 32)                                             // Half way
    elm = (31 - e) << 3;
  else
    elm = (e - 64) << 3;

  uint32_t color = Wheel(w[source]);
  for (byte i = 0; i < 3; ++i) {
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
class toward : public animation {
  public:
    toward()                                              { min_p = 5; max_p = 10; show_time = 6; }
    virtual void init(void)                               { w = random(256); }
    virtual void show(void);
  private:
    byte w;
};

void toward::show(void) {
  int n = strip.numPixels();
  for (int i = n-2; i >= 2; i -= 2) {
    uint32_t c = strip.getPixelColor(i-2);
    strip.setPixelColor(i, c); 
  }
  for (int i = 1; i < n-1; i += 2) {
    uint32_t c = strip.getPixelColor(i+2);
    strip.setPixelColor(i, c);
  }
  if (random(16) == 0) {
    strip.setPixelColor(0,   0);
    strip.setPixelColor(n-1, 0);
    w += 4;
  } else {
    strip.setPixelColor(0,   Wheel(w+128));
    strip.setPixelColor(n-1, Wheel(w));
  }
}

//------------------------------------------- Raindow colors toward each other ------------------------------------------
class towardRain : public animation {
  public:
    towardRain()                                          { min_p = 10; max_p = 20; show_time = 8; }
    virtual void init(void)                               { w = random(256); }
    virtual void show(void);
  private:
    byte w;
};

void towardRain::show(void) {
  int n = strip.numPixels();
  for (int i = n-2; i >= 2; i -= 2) {
    uint32_t c = strip.getPixelColor(i-2);
    strip.setPixelColor(i, c); 
  }
  for (int i = 1; i < n-1; i += 2) {
    uint32_t c = strip.getPixelColor(i+2);
    strip.setPixelColor(i, c);
  }

  if (!random(17)) {
    strip.setPixelColor(0,   0);
    strip.setPixelColor(n-1, 0);
  } else {
    strip.setPixelColor(0,   Wheel(w)); w += 9;
    strip.setPixelColor(n-1, Wheel(w)); w += 3;
  }
}


//---------------------------------------------- Slow single wave moving in random direction -- ---------------------------
class lghtHouse : public BRGTN, public animation {
  public:
    lghtHouse()                                          { min_p = 2; max_p = 10; show_time = 6; }
    virtual void init(void);
    virtual void show(void);
  private:
    uint32_t dot[5];
    int      pos;
    byte     stp;
    char     incr;
    byte     sp;
    byte     dlay;
};

void lghtHouse::init(void) {
  uint32_t c = 0xff;
  for (byte i = 0; i <= 4; ++i) {
    dot[i] = c | (c << 8) | (c << 16);
    c >>= 1;
  } 
  BRGTN::setColor(dot[4]);
  pos    = random(strip.numPixels());
  stp    = 0;
  sp     = random(1, 4);
  dlay   = sp; 
}

void lghtHouse::show() {
  BRGTN::changeAll(-8);
  if (--dlay > 0) return;
  dlay = sp;

  int n = strip.numPixels();
  if (stp <= 0) {
    incr = 1;
    if (pos > n / 2) incr = -1;
    stp = random(5, n);
    sp  += random(3) - 1;
    if (sp < 1)
      sp = 1;
    else if (sp > 3)
      sp = 3;
  }
  pos += incr;
  pos %= n;

  for (int i = 0; i <= 5; ++i) {
    byte indx = i;
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
class rndDrops : public BRGTN, public TWOCLR, public animation {
  public:
    rndDrops()                                            { min_p = 2; max_p = 7; show_time = 9; }
    virtual void init(void)                               { num = 0; add(); }
    virtual void show(void);
  private:
    void add(void);
    struct drop {
      int      pos;
      char     tm;
    };
    struct drop dr[16];
    char num;
};


void rndDrops::show(void) {

  int n = strip.numPixels();
  for (byte i = 0; i < num; ++i) {
    if (++dr[i].tm > 7) {                                 // Delete old drops
      dr[i].pos = dr[byte(num-1)].pos;
      dr[i].tm  = dr[byte(num-1)].tm;
      --num; --i;
      continue;
    }
    int p = dr[i].pos - dr[i].tm;
    if (p < 0) p += n;
    uint32_t c1 = strip.getPixelColor(p+1);
    BRGTN::change(p+1, -64);
    uint32_t c2 = strip.getPixelColor(p);
    c2 = TWOCLR::add(c1, c2);
    strip.setPixelColor(p, c2);

    p = dr[i].pos + dr[i].tm;
    if (p >= n) p -= n;
    c1 = strip.getPixelColor(p-1);
    if (dr[i].tm > 1) BRGTN::change(p-1, -32);
    c2 = strip.getPixelColor(p);
    c2 = TWOCLR::add(c1, c2);
    strip.setPixelColor(p, c2);

    BRGTN::change(dr[i].pos, -64);
  }

  BRGTN::changeAll(-32);
  
  add();
}

void rndDrops::add(void) {
  if (num >= 16) return;
  int pos    = random(strip.numPixels());
  uint32_t c = strip.getPixelColor(pos);
  if (c) return;
  c = Wheel(random(256));
  strip.setPixelColor(pos, c);
  dr[byte(num)].pos = pos;
  dr[byte(num)].tm  = 0;
  num++;
}

// --------------------------------------------- Solitons are creaping up or down -----------------------------------------
class solCreep: public CRAWL, public animation {
  public:
    solCreep(void)                                        { min_p = 6; max_p = 14; show_time = 9; }
    virtual void init(void);
    virtual void show(void);
  private:
    void newSoliton(void);
    int  space;                                           // space between the solitons
    int  change_direction;                                // the time to change direction
    uint32_t dot[5];
    int  sol;
    byte w;
};

void solCreep::init(void) {
  w = random(256);
  CRAWL::fwd = random(2);
  change_direction = random(200, 500);
  newSoliton();
}

void solCreep::show(void) {
  CRAWL::step();

  if (--change_direction <= 0) {
    CRAWL::fwd = !CRAWL::fwd;
    change_direction = random(200, 500);
  }

  int pos = strip.numPixels() - 1;
  if (CRAWL::fwd) pos = 0;

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
  uint32_t c = Wheel(w);
  dot[0] = c;
  w += 71;
  uint32_t r = c & 0xff;
  uint32_t g = (c >> 8)  & 0xff;
  uint32_t b = (c >> 16) & 0xff;
  for (byte i = 1; i <= 4; ++i) {
    r >>= 1;
    g >>= 1;
    b >>= 1;
    uint32_t cc = b & 0xff; cc <<= 8;
    cc |= g & 0xff; cc <<= 8;
    cc |= r & 0xff;
    dot[i] = cc;
  }
  space = random(3, 10);

  int pos = strip.numPixels() - 1;
  if (CRAWL::fwd) pos = 0;
  strip.setPixelColor(pos, dot[4]);
}

//---------------------------------------------- Classes for strip clearing  ----------------------------------------------
class clr : public BASE {
  public:
    clr()                                                 {}
    virtual void  init(void) = 0;
    virtual void  show(void) = 0;
    bool          isComplete(void) { return complete; }
    bool          fade(uint16_t index, byte val);
    bool          fadeAll(byte val);
  protected:
    bool complete;
};

bool clr::fadeAll(byte val) {
  bool finish = true;
  for (uint16_t i = 0; i < strip.numPixels(); ++i) {
    if (!fade(i, val)) finish = false;
  }
  return finish;
}

bool clr::fade(uint16_t index, byte val) {
  uint32_t c = strip.getPixelColor(index);
  byte bound = 0;
  for (char s = 16; s >= 0; s -= 8) {
    long cc = c >> s;                                     // The color component (red, green or blue)
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

// --------------------------------------------- Clear the strip from the ether side --------------------------------------
class clearSide : public clr {
  public:
    clearSide()                                           {}
    virtual void init(void);
    virtual void show(void);
  private:
    uint32_t color;
    int      index;
    bool     fwd;
};

void clearSide::init(void) {
  complete = false;
  color    = Wheel(random(256));
  fwd      = random(2);
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
class clearCntr : public clr {
  public:
    clearCntr()                                           {}
    virtual void init(void);
    virtual void show(void);
  private:
    uint32_t color;
    int      l, r;
    bool     fwd;
};

void clearCntr::init(void) {
  complete = false;
  color = Wheel(random(256));
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

// --------------------------------------------- Clear the strip from the ether side --------------------------------------
class clearFade : public clr {
  public:
    clearFade()                                           {}
    virtual void init(void)                               { complete = false; decrement = random(4, 16); }
    virtual void show(void)                               { complete = clr::fadeAll(decrement); }
  private:
    byte decrement;
};

// --------------------------------------------- Clear the strip by 'eating' the pixels from the center -------------------
class eatCntr : public clr {
  public:
    eatCntr()                                             {}
    virtual void init(void)                               { complete = false; remain = strip.numPixels()/2 + 1;}
    virtual void show(void);
  private:
    int remain;
};

void eatCntr::show(void) {
  int n = strip.numPixels();
  int c = n / 2;
  for (int i = c; i > 0; --i) {
    uint32_t c = strip.getPixelColor(i-1);
    strip.setPixelColor(i, c);
  }
  for (int i = c; i < n-1; ++i) {
    uint32_t c = strip.getPixelColor(i+1);
    strip.setPixelColor(i, c);
  }
  strip.setPixelColor(0, 0);
  strip.setPixelColor(n-1, 0);
  --remain;
  complete = (remain <= 0);
}

//---------------------------------------------- Clear the strip by deviding it by 2 ------------------------------------
class clearHalf :  public clr {
  public:
    clearHalf()                                           {}
    virtual void init(void);
    virtual void show(void);
  private:
    int  one_step;
};

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

//---------------------------------------------- Shuffle the animation in the random order --------------------------------
class shuffle {
  public:
    shuffle(byte a_size) {
      for (byte i = 0; i < a_size; ++i) index[i] = i;
      curr = num_anim = a_size; 
    }
    byte  next(void);
  private:
    void  randomize(void);
    byte  index[34];                                      // The maximum number of animations
    byte  num_anim;                                       // The active animation number
    byte  curr;
};

byte shuffle::next(void) {
  if (curr >= num_anim) randomize();
  return index[curr++];
}

void shuffle::randomize(void) {
  for (byte i = 0; i < num_anim*2; ++i) {
    byte p1 = random(num_anim);
    byte p2 = random(num_anim);
    if (p1 != p2) {
      byte t    = index[p2];
      index[p2] = index[p1];
      index[p1] = t;
    }
  }
  curr = 0;
}

// --------------------------------------------- The sequence manager -----------------------------------------------------
class MANAGER : public shuffle {
  public:
    MANAGER(animation* a[], byte a_size, clr* c[], byte clr_size) : shuffle(a_size) {
      anims     = a;
      clearance = c;
      num_clr   = clr_size;
      stp       = 0;
      do_clear  = false;
      aIndex    = 0;
    }
    void        init(void);
    void        show(void);
    void        menu(void)                              { stp_period --; if (stp_period < 1) stp_period = 1; }
    void        menu_l(void)                            { initClear(); }
    void        incr(void)                              { stp_period ++; if (stp_period > 20) stp_period = 20; }
  private:
    void        initClear(void);
    bool        isClean(void);
    animation** anims;
    clr**       clearance;
    byte        num_clr;
    uint32_t    stp = 0;
    uint16_t    stp_period;
    uint16_t    clr_stp_period;
    uint32_t    next;
    byte        aIndex;
    animation*  a;
    clr*        c;
    bool        do_clear;                                // Whether cleaning the strip
};

void MANAGER::init(void) {
  if (!anims[aIndex]->do_clear) {
    aIndex = shuffle::next();
    //aIndex = 18;
    a = anims[aIndex];

    uint32_t period = a->show_time;
    period = random(period, period * 3);                  // time in 10-seconds intervals
    period *= 10000;                                      // Now time in ms
    next = millis() + period;
  }

  a = anims[aIndex];
  uint16_t min_stp = a->min_p * 10;
  uint16_t max_stp = a->max_p * 10;
  stp_period = random(min_stp, max_stp+1);
  stp = 0;

  strip.clear();
  a->do_clear = false;
  a->init();
  strip.show();
  a->do_clear = false;
}

void MANAGER::show(void) {
  uint32_t ms = millis();
  if (!do_clear && (ms > next) && a->complete) {          // The current animation is timed out
    if (isClean())
      init();
    else
      initClear();
  }
  
  if (ms < stp) { delay(1); return; }
  if (do_clear)
    stp = ms + clr_stp_period;
  else
    stp = ms + stp_period;

  if (do_clear) {
    if (c->isComplete()) {
      do_clear = false;
      if (ms > next) a->do_clear = false;                 // It is too late to continue the animation
      init();
    } else
      c->show();                                          // Keep running clear session till it ends
  } else {
    if (a->do_clear) initClear();
    a->show();
  }
  strip.show();
}

void MANAGER::initClear(void) {
   do_clear = true;                                       // Start clearing sequence
   byte ni = random(num_clr);
   c = clearance[ni];
   c->init();
   stp = 0;
   clr_stp_period = random(3, 10) * 10;
}

bool MANAGER::isClean(void) {
  for (uint16_t i = 0; i < strip.numPixels(); ++i)
    if (strip.getPixelColor(i)) return false;
  return true;
}

//================================ Ent of class definitions ===============================================================
colorWipe   cWipe;
colorWalk   cWalk;
randomCreep cCreep;
rainbow     cRainbow;
rainCycle   cRainCycle;
rainFull    cRainFull;
colorWave   cWave;
lightUp     lUp;
sparks      sRnd;
rndFade     rFade;
centerRun   cRun;
shineSeven  cSeven;
walkSeven   wSeven;
flashSeven  fSeven;
shineFlash  cFlash;
mergeOne    mOne;
mergeWave   mWave;
collideOne  cOne;
neoFire     cFire;
evenOdd     cEvenOdd;
collMdl     cCollMdl;
collEnd     cCollEnd;
rainBlend   cBlend;
swing       cSwing;
swingSingle cSwingS;
randomFill  rFill;
singleWave  sWave;
worms       cWorms;
interfer    cInterf;
toward      cToward;
towardRain  rToward;
lghtHouse   lHouse;
rndDrops    cDrops;
solCreep    sCreep;

clearSide   clrSide;
clearCntr   clrCntr;
clearFade   clrFade;
eatCntr     clrEat;
clearHalf   clrHalf;
animation*  anims[] = {
              &cWipe, &cWalk, &cCreep, &cRainbow, &cRainCycle, &cRainFull, &cWave, &lUp, &sRnd, &rFade,
              &cCollEnd, &cRun, &cSeven, &mOne, &mWave, &cOne, &cFire, &cEvenOdd, &rFill, &cCollMdl,
              &cBlend, &cSwing, &cSwingS, &cFlash, &sWave, &cWorms, &cInterf, &cToward, &rToward, &lHouse,
              &cDrops, &wSeven, &fSeven, &sCreep
            };
clr*        clearance[] = { &clrSide, &clrCntr, &clrFade, &clrEat, &clrHalf };
const byte  num_anim = sizeof(anims) / sizeof(animation*);
const byte  num_clr  = sizeof(clearance) / sizeof(clr*);

MANAGER     mgr(anims, num_anim, clearance, num_clr);

void setup() {
  //Serial.begin(9600);

  uint32_t rnd = 0;
  for (byte i = A0; i < A4; ++i) {
    pinMode(i, INPUT);
    rnd <<= 6;
    rnd += analogRead(i);
  }
  randomSeed(rnd);

  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif

  strip.begin();
  strip.setBrightness(NEO_BRGHT);
  strip.show();                                           // Initialize all pixels to 'off'
  mgr.init();

}

void loop() {
  mgr.show();
}
