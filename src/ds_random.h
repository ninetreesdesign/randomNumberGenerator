// ds_random.h -- portable 16-bit pseudo-random numbers
//
// 32-bit xorshift state, triple (13,17,5), period 2^32-1.
// Runs unchanged on 8-bit AVR, 16-bit and 32-bit MCUs.
//
// Credits:
//   xorshift algorithm and the (13,17,5) triple: George Marsaglia,
//   "Xorshift RNGs", Journal of Statistical Software 8(14), 2003,
//   doi:10.18637/jss.v008.i14.
//   Gaussian approximation by summing 12 uniforms (Irwin-Hall):
//   J. O. Irwin, Biometrika 19(3-4):225-239, 1927,
//   doi:10.1093/biomet/19.3-4.225;
//   P. Hall, Biometrika 19(3-4):240-244, 1927,
//   doi:10.1093/biomet/19.3-4.240.
//
// VERSION: 1.0.0  2026-11-13

#pragma once
#include <stdint.h>

// Set the generator state. seed == 0 selects the default non-zero seed.
// micros() is still mixed into the state on every call, so a seeded
// sequence is not bit-reproducible.
void rndSeed(uint32_t seed);

// Uniform 32-bit sample.
uint32_t rnd32(void);

// Uniform 16-bit sample, 0..65535 (bits 31..16 of the state).
uint16_t rnd(void);

// Uniform value in low..hi inclusive, written to *out. The bounds may be
// given in either order and must lie within -32768..32767; returns false
// (leaving *out untouched) if they do not.
bool rndBetween(int32_t low, int32_t hi, int16_t *out);

// Gaussian sample, mean 0, standard deviation 1 (Irwin-Hall, 12 uniforms).
// Returns about -6..+6; excess kurtosis is -0.1 rather than 0.
float rndGaussian(void);
