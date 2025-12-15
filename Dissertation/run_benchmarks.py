import os
import subprocess

'''
What do i want to achieve with this file
- Automate running the benchmarks
    - i want to store all the result for the different operations in different files
    - maybe run the test in the background



'''

bench_beap="bench_beap"
bench_heap="bench_heap"
bench_bst="bench_bst"

bench_dir = "bin"
bench_result_dir = "benchmark_results"
BENCH_BINARIES = {
    "beap": "bench_beap",
    "heap": "bench_heap",
    "bst": "bench_bst",
    #"nbeap": "bench_nbeap",
}

BENCHMARKS = [
    "BM_Baseline", "BM_PushRandom", "BM_PushSortedAsc",
    "BM_PushSortedDesc", "BM_Pop", "BM_Search", 
    "BM_RemoveValue"
    ]


def checkFiles():
    # check the each benchmark files exist otherwise return
    for bench, file in BENCH_BINARIES.items():
        try:
            path = os.path.join(bench_dir, file)
            print(path)
            with open(path, "r"):
                print(f"Reading {path}")
        except FileNotFoundError:
            print(f'{path} does not exist, compiling {path}')
            # run make bench-{bench}
            try:
                subprocess.run(["make", f"bench-{bench}"], check=True)
                print(f"Successfully compiled {bench} benchmark")
            except subprocess.CalledProcessError as e:
                print(f'Failed to compile {bench} benchmark: {e}')


def cleanCSVOutput(path):
    try:
        with open(path, "r") as f:
            lines = f.readlines()[9:]
        
        with open(path, 'w') as f:
            f.writelines(lines)
    except FileNotFoundError:
        print(f"Unable to clean csv file: {path}")


def runEachBenchmark(bench, file):
    path = os.path.join(bench_dir, file)
    for bm in BENCHMARKS:
        print(f"Running {bm} benchmark for {bench}")
        resultPath = os.path.join(bench_result_dir, f'bench_{bench}_{bm}.csv')
        try:
            subprocess.run(
                [
                    f'./{path}', 
                    f'--benchmark_filter=^{bm}($|/)',
                    f"--benchmark_out={resultPath}",
                    "--benchmark_out_format=csv",
                    "--benchmark_display_aggregates_only=true",
                    #"--benchmark_context=false", 
                ]
                , check=True 
            )
            cleanCSVOutput(resultPath)
        except subprocess.CalledProcessError as e:
            print(f'Failed to run {bm} benchmarks for {bench}')

        
def runBenchMarks():
    # for each benchmark file
    # for each benchmark
    # run the benchmark and direct the output to a csv file
    for bench, file in BENCH_BINARIES.items():
        runEachBenchmark(bench, file)

    pass

def globalRun():
    # make output dir if not present
    os.makedirs(bench_result_dir, exist_ok=True)
    # check if the necessary files are available 
    checkFiles()

    # run the files
    runBenchMarks()



if __name__ == "__main__":
    globalRun()