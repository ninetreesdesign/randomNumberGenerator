// ds_random.cpp -- portable 16-bit pseudo-random numbers
//
// VERSION: 1.0.0  2026-11-13

#include "ds_random.h"
#include <Arduino.h>

static uint32_t s_state = 0x9E3779B9u;

void rndSeed(uint32_t seed) {
  s_state = seed ? seed : 0x9E3779B9u;
}

// xorshift32 with the (13,17,5) triple. Marsaglia, "Xorshift RNGs",
// Journal of Statistical Software 8(14), 2003, doi:10.18637/jss.v008.i14.
uint32_t rnd32(void) {
  s_state += (uint32_t)micros();
  if (s_state == 0) {                 // xorshift cannot leave the zero state
    s_state = 0x9E3779B9u;
  }
  s_state ^= s_state << 13;
  s_state ^= s_state >> 17;
  s_state ^= s_state << 5;
  return s_state;
}

uint16_t rnd(void) {
  return (uint16_t)(rnd32() >> 16);
}

bool rndBetween(int32_t low, int32_t hi, int16_t *out) {
  if (low > hi) {
    int32_t t = low;
    low = hi;
    hi = t;
  }
  if (low < -32768L || hi > 32767L) {
    return false;                     // range does not fit a 16-bit number
  }

  uint32_t span  = (uint32_t)(hi - low) + 1u;       // 1 .. 65536
  uint32_t limit = 65536u - (65536u % span);        // whole multiples only
  uint32_t v;

  do {
    v = rnd();
  } while (v >= limit);

  *out = (int16_t)(low + (int32_t)(v % span));
  return true;
}

// Irwin-Hall approximation: the sum of 12 uniform variates, scaled so the
// standard deviation is 1. See Irwin (1927) and Hall (1927), Biometrika
// 19(3-4); 12 terms is the standard choice for unit variance.
float rndGaussian(void) {
  uint32_t sum = 0;

  for (uint8_t i = 0; i < 6; i++) {   // 6 words = 12 x 16-bit uniforms
    uint32_t r = rnd32();
    sum += r & 0xFFFF;
    sum += r >> 16;
  }

  return ((int32_t)sum - 393210) * (1.0f / 65536.0f);   // mean 12*32767.5, sd 65536
}
