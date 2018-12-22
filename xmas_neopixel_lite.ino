#include "neopixel.h"
#include "clrutils.h"
#include "animation.h"
#include "clean.h"
#include "manager.h"

const uint8_t   NEO_BRGHT    = 255;
const uint8_t   NEO_PIN      = 6;                           // Pin of Neopixel Strip
const uint16_t  StripSize    = 100;                         // Length of Neopixel Strip

NEOPIXEL strip = NEOPIXEL(StripSize, NEO_PIN, NEO_RGB + NEO_KHZ800);

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
//theatChase  tChase;
//meteorSky   mtrSky;
symmRun     symRun;
//metSingle   mtrSgl;
//pureStrip   pStrip;
//sideFill    sFill;
//browMotion  cBrowian;
//rainDrops   cRainDrp;
ripeFruit   cRipe;
//brightWave  bWave;
//brColCreep  bcCreep;

clearSide   clrSide;
clearCntr   clrCntr;
clearFade   clrFade;
eatCntr     clrEat;
clearHalf   clrHalf;
animation*  anims[] = {
              &cWipe, &cWalk, &cCreep, &cRainbow, &cRainCycle, &cRainFull, &cWave, &lUp, &sRnd, &rFade,
              &cCollEnd, &cRun, &cSeven, &mOne, &mWave, &cOne, &cFire, &cEvenOdd, &rFill, &cCollMdl,
              &cBlend, &cSwing, &cSwingS, &cFlash, &sWave, &cWorms, &cInterf, &cToward, &rToward, &lHouse,
              &cDrops, &wSeven, &fSeven, &sCreep, &symRun, &cRipe
            };
clr*        clearance[] = { &clrSide, &clrCntr, &clrFade, &clrEat, &clrHalf };
const byte  num_anim = sizeof(anims) / sizeof(animation*);
const byte  num_clr  = sizeof(clearance) / sizeof(clr*);

MANAGER     mgr(anims, num_anim, clearance, num_clr);

void setup() {
    //Serial.begin(115200);

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
