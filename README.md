# xtmas_neopixel
Updated DEC 13 2018. In this new update the code was revisited and new animation programs were added. Not all animations are enabled in the source code, some of there are commented out to decrease memory requirements. But many animations are beautiful, so i kept them all. Please, look at xtmas_neopixel_lite.ino file and select your own animation set. To do this, first comment out or remove comment from animation definition instance
symmRun     symRun;
//metSingle   mtrSgl;
Then add or remove the reference to animation class to anims array. Be carefull of warnings about memory limit if Arduino IDE. The program can be unstable in case of insufficient memory amount. 

animation*  anims[] = {
              &cWipe, &cWalk, &cCreep, &cRainbow, &cRainCycle, &cRainFull, &cWave, &lUp, &sRnd, &rFade,
              &cCollEnd, &cRun, &cSeven, &mOne, &mWave, &cOne, &cFire, &cEvenOdd, &rFill, &cCollMdl,
              &cBlend, &cSwing, &cSwingS, &cFlash, &sWave, &cWorms, &cInterf, &cToward, &rToward, &lHouse,
              &cDrops, &wSeven, &fSeven, &sCreep, &tChase, &mtrSky, &symRun, &cRipe
            };

The article is available here: https://www.hackster.io/sfrwmaker/5-minute-christmas-neopixel-led-strip-0abf33

In case of issues, the previous release is available in OLD directory.
