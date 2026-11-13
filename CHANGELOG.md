# Changelog

All notable changes to this project are documented here. The format follows
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and the project uses
[semantic versioning](https://semver.org/).

## [1.0.0] - 2026-11-13

### Added

- `rnd()`, `rnd32()`, `rndBetween()`, `rndSeed()`, `rndGaussian()`.
- Examples: `minimal`, `gaussian`, `dac_out`.
- MIT license.
- `CITATION.cff`.

### Changed

- The repository license changed from GPL-3.0 (2018 sketch) to MIT.
- The repository is now an Arduino library. It replaces the 2018
  `ds_rng_XORshift.ino` sketch, which returned 0 on 8-bit AVR because
  `unsigned int` cannot hold 65536.
