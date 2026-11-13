# ds_random — Arduino random number generator

`ds_random` is a portable pseudo-random number generator (PRNG) library for
Arduino. It returns 16-bit uniform samples, a checked signed range, and a
Gaussian approximation, from a 32-bit xorshift state with a period of 2³²−1.
It runs on 8-bit AVR and 32-bit cores, with no architecture-specific code.

- 32-bit xorshift state with the `(13,17,5)` triple, period 2³²−1.
- Uniform 16-bit and 32-bit samples, signed ranges, Gaussian noise.
- Plain function calls; no object, no `begin()`.

`rnd` rather than `rand`: `rand()` is a C standard library function
declared in `<stdlib.h>`, which `Arduino.h` includes, so a function of that
name would collide. `rnd` is free.

## Use cases

- **Audio and synth noise** — white noise from `rnd()`, Gaussian from
  `rndGaussian()`.
- **Filter response testing** — drive a hardware filter with white noise and
  measure its output; see [Bandwidth and filter testing](#bandwidth-and-filter-testing).
- **Dithering and randomised test vectors** — repeatable seeds via `rndSeed()`.
- **Games and simulation** — ranges with `rndBetween()`, no modulo bias.

## Why not Arduino's `random()`?

Arduino's `random(lo, hi)` is a 32-bit linear congruential generator followed
by a modulo. On AVR that modulo is a 32-bit division, which dominates the
cost; it returns a `long`, has no signed-range helper, and no Gaussian option.

`ds_random` uses shifts and XOR, rejects rather than modulo-reduces so
`rndBetween()` is unbiased, and adds `rndGaussian()`. If you only need
`random(0, 100)` and are not timing it, the built-in is fine.

## Install

- **Arduino Library Manager** — search for `ds_random` (available once the
  library is registered).
- **ZIP** — download the repository and use *Sketch > Include Library >
  Add .ZIP Library*.
- **Manual** — copy the folder into your sketchbook `libraries/` directory.

Restart the IDE. The examples appear under *File > Examples > ds_random*.

## Compatibility

No assembly, no endian assumptions, no word-size assumptions: the library
uses `uint32_t`, shifts, XOR and `micros()`.

| Core | Verified |
|---|---|
| AVR 8-bit, ATmega328P (Uno/Mega) | compiles, avr-gcc 7.3.0 |
| Teensy 3.2, ARM Cortex-M4 | compiles, arm-none-eabi 11.3.1 |
| Teensy 4.x, ESP32, SAMD, RP2040 | same portable C++; not built here |
| 16-bit MSP430 | not tested |

`architectures=*` in `library.properties` follows the Arduino convention for
"all"; the table says what has actually been built.

## API

| Call | Returns |
|---|---|
| `rndSeed(uint32_t seed)` | sets the state; `0` selects the default non-zero seed |
| `rnd32()` | uniform 32-bit sample |
| `rnd()` | uniform 16-bit sample, 0..65535 (bits 31..16 of the state) |
| `bool rndBetween(int32_t low, int32_t hi, int16_t *out)` | uniform signed value in `low..hi`; `false` if the bounds do not fit −32768..32767 |
| `float rndGaussian()` | Gaussian, mean 0, standard deviation 1, about −6..+6 |

`rndBetween()` takes signed 32-bit bounds so it can reject a range that does
not fit a 16-bit number, and writes the result through `out`; `*out` is left
untouched when it returns false. Bounds may be given in either order, and
rejection sampling keeps every value equally likely.

`rndGaussian()` sums 12 uniforms (six `rnd32()` calls, two 16-bit halves
each) and scales by 2⁻¹⁶. The result is a good Gaussian with excess kurtosis
−0.1; the tails stop at ±6.

```cpp
#include <ds_random.h>

uint16_t a = rnd();            // 0..65535

int16_t v;
if (rndBetween(-1000, 1000, &v)) {
  // v is -1000..1000
}

float g = rndGaussian();       // mean 0, sd 1
```

`rndSeed()` sets the state, but `micros()` is still mixed in on every call,
so a seeded sequence is not bit-reproducible across runs.

## Performance

Arduino's `random(lo, hi)` pays for a 32-bit multiply and a 32-bit modulo on
AVR; this generator avoids both. The 2015 AVR benchmark (Teensy 2, 16-bit
variant) measured about 10× faster than `random(lo, hi)` there.

Treat that ratio as historical. It depends on the Arduino core and the
compiler version — Arduino builds with a general-purpose avr-gcc — and the
32-bit variant here is larger than the 2015 one, so re-measure before
quoting it. As a current reference, the 32-bit variant costs roughly 350
cycles per call on a 16 MHz AVR (~22 µs); that is an instruction-count
estimate from the disassembly with Arduino AVR core 1.8.7 / avr-gcc 7.3.0,
not a runtime measurement.

## Bandwidth and filter testing

`rnd()` is white: the sample stream has a flat spectrum up to the output
rate's Nyquist limit. What limits a filter measurement is the output stage,
not the generator.

- Output rate: `fs = 1e6 / SAMPLE_PERIOD_US`.
- Usable band: `fs / 2` (Nyquist).
- To measure a filter with bandwidth `B`, run `fs` at least `2B`, and well
  above it (5-10x) when the transition band and stopband matter.
- The DAC's zero-order hold adds a sinc rolloff and images at multiples of
  `fs`, so keep the filter band well below `fs / 2`.

A bandwidth parameter would be a convenience in the output sketch, not in
this library: the generator only produces samples, the sketch owns timing.

## Examples

- **`minimal`** — call `rnd()` in a loop and print each sample.
- **`gaussian`** — call `rndGaussian()` in a loop and print each sample.
- **`dac_out`** — Teensy 3.2, 12-bit DAC on A14, one sample every
  `SAMPLE_PERIOD_US` with `rnd() >> 4`.

The `minimal` and `gaussian` examples include `waitForSerial()`, the Teensy
USB timeout: it waits up to 5 s for the serial monitor and then continues,
so the sketch runs with no monitor attached. It is marked Teensy only —
on AVR the test is always true and returns at once.

## FAQ

**Is ds_random cryptographic?**
No. It is a statistical PRNG for signals, test vectors and simulation. Use a
hardware RNG or a CSPRNG for keys, tokens or anything an attacker must not
predict.

**Which boards are supported?**
8-bit AVR (ATmega328P, Uno/Mega) and 32-bit ARM (Teensy 3.2) are compiled and
checked. Teensy 4.x, ESP32, SAMD and RP2040 use the same portable C++ but have
not been built here. See [Compatibility](#compatibility).

**How do I get a number in a range?**
`rndBetween(low, hi, &out)` returns `false` if the range does not fit a signed
16-bit number, and never modulo-reduces, so the values are unbiased.

**How do I get Gaussian noise?**
`rndGaussian()` returns mean 0, standard deviation 1, from an Irwin-Hall sum
of 12 uniforms.

**How do I seed it?**
`rndSeed(value)`. `micros()` is still mixed in on every call, so a seeded
sequence is not bit-reproducible.

**What is the period?**
2³²−1 from any non-zero seed.

**Why is the function called `rnd` and not `rand`?**
`rand()` is declared in `<stdlib.h>`, which `Arduino.h` includes; defining it
would collide.

## Notes

- The 32-bit state is deliberate. On AVR, `unsigned int` is 16 bits and a
  16-bit xorshift state with this triple repeats every 558 samples.
- To trade period for speed on an 8-bit MCU, change `ds_random.cpp` to a
  `uint16_t` state with the `(7,9,8)` triple and return the state directly.
  The period then drops to 65,535 samples.
- The generator algorithm is an implementation detail and may change in a
  later version; the function signatures are the stable part.
- Not suitable for cryptographic use.

## Roadmap

- Bandwidth helper for the output example (derive `SAMPLE_PERIOD_US` from a
  requested `fs`).
- Pink noise (`rndPink()`, Voss-McCartney) as a separate function over
  `rnd()`, not a mode argument.
- Other amplitude distributions beside `rndGaussian()`.

## Credits

- **xorshift** algorithm and the `(13,17,5)` triple: George Marsaglia,
  "Xorshift RNGs", *Journal of Statistical Software* 8(14), 2003.
  [doi:10.18637/jss.v008.i14](https://doi.org/10.18637/jss.v008.i14)
- **Gaussian approximation** (sum of 12 uniforms, the Irwin-Hall distribution):
  - J. O. Irwin, "On the frequency distribution of the means of samples from
    a population having any law of frequency with finite moments, with special
    reference to Pearson's Type II", *Biometrika* 19(3-4), 1927, pp. 225-239.
    [doi:10.1093/biomet/19.3-4.225](https://doi.org/10.1093/biomet/19.3-4.225)
  - P. Hall, "The distribution of means for samples of size N drawn from a
    population in which the variate takes values between 0 and 1, all such
    values being equally probable", *Biometrika* 19(3-4), 1927, pp. 240-244.
    [doi:10.1093/biomet/19.3-4.240](https://doi.org/10.1093/biomet/19.3-4.240)
- The original 16-bit sketch this library grew from adapted code by
  Daniel Dunn (engineeringnotes.blogspot.com).

## License

MIT. See [LICENSE](LICENSE).

## Version

1.0.0 — 2026-11-13
