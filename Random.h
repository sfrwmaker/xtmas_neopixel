#ifndef __RANDOM_H
#define __RANDOM_H

inline uint32_t Random(uint32_t a, uint32_t b)  { return random(a, b); }
inline uint32_t Random(uint32_t a)              { return random(a); }

#endif
