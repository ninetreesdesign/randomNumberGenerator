// dac_out -- timed 16-bit noise on a 12-bit DAC
//
// Teensy 3.2: DAC on A14. One sample every SAMPLE_PERIOD_US; the 16-bit
// value from rnd() is scaled to 12 bits with >> 4.
//
// fs = 1e6 / SAMPLE_PERIOD_US; the usable band is fs/2. For filter-response
// testing, set SAMPLE_PERIOD_US so fs is well above the filter bandwidth.
//
// Other boards: change DAC_PIN and DAC_BITS. Boards without a DAC need
// PWM instead, with a smaller shift (8 bits is typical).

#include <ds_random.h>

static const uint32_t SAMPLE_PERIOD_US = 1000;   // 1 kHz; 500 = 2 kHz
static const uint8_t  DAC_BITS         = 12;     // Teensy 3.2 DAC
static const uint8_t  DAC_PIN          = A14;
static const uint8_t  SAMPLE_SHIFT     = 16 - DAC_BITS;   // 4

static uint32_t next_us = 0;

void setup() {
  analogWriteResolution(DAC_BITS);
  next_us = (uint32_t)micros();
}

void loop() {
  uint32_t now_us = (uint32_t)micros();

  if ((int32_t)(now_us - next_us) < 0) {
    return;                             // not time yet
  }

  next_us += SAMPLE_PERIOD_US;
  if ((int32_t)(now_us - next_us) >= 0) {
    next_us = now_us + SAMPLE_PERIOD_US;   // fell behind; drop missed slots
  }

  analogWrite(DAC_PIN, rnd() >> SAMPLE_SHIFT);
}
