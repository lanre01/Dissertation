# Dissertation Project

This repository contains the code, benchmarks, and visualisation assets for a dissertation focused on beaps and n-dimensional beaps implemented in C++.

## Overview

The main implementation lives in `Dissertation/` and includes:

- `Beap` in `src/beap/beap.hpp`
- `NBeap` in `src/n_beap/n_beap.hpp`
- unit tests in `test/unit/`
- benchmark programs in `test/benchmark/`
- benchmark output in `benchmark_results/`
- cached benchmark summaries via `cache.py` in `bench_results_cache/`
- visualisation files in `Visuals/` and `Dissertation/images/`

## Requirements

- C++20-compatible compiler
- `make`
- Python 3
- `pandas` for `cache.py`
- Google Benchmark `1.9.4`
- GoogleTest `1.17.0`

The current Makefile is configured to use `clang++`.

## Build And Run

All build commands should be run from the `Dissertation/` directory.

```bash
cd Dissertation
make
make run
```

Useful commands:

- `make tests` builds the unit test binary
- `make run-unit-tests` builds and runs all unit tests
- `make run-beap-tests` runs only `Beap` tests
- `make run-nbeap-tests` runs only `NBeap` tests
- `make bench-all` builds benchmark binaries
- `python3 run_benchmarks.py` runs the benchmark suite and writes CSV output to `benchmark_results/`
- `python3 cache.py <benchmark-name> [label]` extracts summary timing data from `benchmark_results/` and appends it to `bench_results_cache/`

## Repository Layout

```text
.
|-- README.md
|-- Dissertation/
|   |-- Makefile
|   |-- src/
|   |-- test/
|   |-- benchmark_results/
|   |-- bench_results_cache/
|   |-- cache.py
|   |-- run_benchmarks.py
|-- Visuals/
```

## Notes

- The root of the implementation is `Dissertation/`, not the repository root
- `cache.py` is used to retain intermediate benchmark summaries so repeated benchmark runs can be compared during analysis
- The project is licensed under the Boost Software License 1.0; see `LICENSE`
