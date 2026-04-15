#!/usr/bin/env python3

import os
import subprocess
import sys 

"""
Benchmark Runner Script

This script automates the execution of Google Benchmark binaries for multiple
data structures and collects results in CSV format.

Supported benchmarks:
    - beap
    - heap
    - bst
    - nbeap
    - minmax_heap
    - dary

Functionality:
    - Automatically checks if benchmark binaries exist
    - Compiles missing benchmarks using `make bench-<name>`
    - Runs benchmarks with optional filtering
    - Outputs results as CSV files in `benchmark_results/`
    - Cleans CSV headers for easier post-processing

Usage:

1. Run all benchmarks:
    python run_benchmarks.py

2. Run a single benchmark for non template benchmark files:
    python run_benchmarks.py <benchmark_name>

3. Run a benchmark over a range of dimensions:
    python run_benchmarks.py <benchmark_name> <lower> <upper>

    Example:
    python run_benchmarks.py nbeap 1 10
    python run_benchmarks.py dary 2 2
"""

bench_dir = "bin"


class BenchmarkRunner():
    def __init__(self):
        self.BENCH_BINARIES = {
                "beap": "bench_beap",
                "heap": "bench_heap",
                "bst": "bench_bst",
                "nbeap": "bench_nbeap",
                "minmax_heap" : "bench_minmax_heap",
                "dary_heap": "bench_dary_heap"
            }
        self.bench_dir = "bin"
        self.bench_result_dir = "benchmark_results"
        os.makedirs(self.bench_result_dir, exist_ok=True)
        self.checkFiles()
        self.dim = 0
    
    def present(self, bench):
        return bench in self.BENCH_BINARIES
    
    def checkFiles(self):
        for bench, file in self.BENCH_BINARIES.items():
            try:
                path = os.path.join(self.bench_dir, file)
                print(path)
                with open(path, "r"):
                    print(f"Reading {path}")
            except FileNotFoundError:
                print(f'{path} does not exist, compiling {path}')
                try:
                    subprocess.run(["make", f"bench-{bench}"], check=True)
                    print(f"Successfully compiled {bench} benchmark")
                except subprocess.CalledProcessError as e:
                    print(f'Failed to compile {bench} benchmark: {e}')

    def cleanCSVOutput(self, path):
        try:
            with open(path, "r") as f:
                lines = f.readlines()[9:]
            
            with open(path, 'w') as f:
                f.writelines(lines)
        except FileNotFoundError:
            print(f"Unable to clean csv file: {path}")


    def runAll(self):
        for bench in self.BENCH_BINARIES:
            pattern = ""
            if bench == "nbeap":
                self.runRange(bench, 1, 10)
            elif bench == "dary_heap":
                self.runRange(bench, 2, 10)
            else:
                self.run(bench, pattern)

    
    def run(self, bench, pattern):
        benchmarkFile = self.BENCH_BINARIES[bench]
        path = os.path.join(self.bench_dir, benchmarkFile)
    
        print(f"Running benchmark for {bench}")
        resultPath = ""
        if pattern:
            resultPath = os.path.join(self.bench_result_dir, f'bench_{self.dim}-{bench}.csv')
        else:
            resultPath = os.path.join(self.bench_result_dir, f'bench_{bench}.csv')

        try:
            subprocess.run(
                [
                    f'./{path}', 
                    f'--benchmark_filter={pattern}',
                    f"--benchmark_out={resultPath}",
                    "--benchmark_out_format=csv",
                    "--benchmark_repetitions=10",
                    "--benchmark_display_aggregates_only=true"
                ]
                , check=True 
            )
            self.cleanCSVOutput(resultPath)
        except subprocess.CalledProcessError as e:
            print(f'Failed to run benchmarks for {bench}')
        
    
    def runRange(self, bench, lowerBoundary, upperBoundary):
        for N in range(lowerBoundary, upperBoundary+1):
            pattern = None
            if bench == "dary_heap":
                pattern = rf"<{N}>"
            else:
                pattern = rf"^BM_.*<[^>]*,[[:space:]]*{N}>($|/)"
            print("--benchmark_filter=", pattern)
            self.dim = N # setting this here to be used to name the output file
            self.run(bench, pattern)
        



if __name__ == "__main__":  
    argSize = len(sys.argv[1:])
    br = BenchmarkRunner()
    if argSize == 1 and br.present(sys.argv[1]):
        br.run(sys.argv[1], "")
    elif argSize > 1:
        br.runRange(sys.argv[1], int(sys.argv[2]), int(sys.argv[3]))
    else:
        br.runAll()