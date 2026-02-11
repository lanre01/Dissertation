import os
import subprocess
import sys 

'''
What is the plan?
- I want to be able to run each benchmark result one at a time.
- means the script needs to accept some arguments
- I want to use a class this time
'''
bench_dir = "bin"


class BenchmarkRunner():
    def __init__(self):
        self.BENCH_BINARIES = {
                "beap": "bench_beap",
                "heap": "bench_heap",
                "bst": "bench_bst",
                "nbeap": "bench_nbeap",
            }
        self.bench_dir = "bin"
        self.bench_result_dir = "benchmark_results"
        os.makedirs(self.bench_result_dir, exist_ok=True)
        self.checkFiles()
        self.nbeapDim = 0
        pass 
    
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
                # run make bench-{bench}
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
                continue
            self.run(bench, pattern)
        pass 
    
    def run(self, bench, pattern): # need to update this to accept template parameters for NBeap
        benchmarkFile = self.BENCH_BINARIES[bench]
        path = os.path.join(self.bench_dir, benchmarkFile)
    
        print(f"Running benchmark for {bench}")
        resultPath = ""
        if pattern:
            resultPath = os.path.join(self.bench_result_dir, f'bench_{self.nbeapDim}-{bench}.csv')
        else:
            resultPath = os.path.join(self.bench_result_dir, f'bench_{bench}.csv')

        try:
            subprocess.run(
                [
                    f'./{path}', 
                    f'--benchmark_filter={pattern}',
                    f"--benchmark_out={resultPath}",
                    "--benchmark_out_format=csv",
                    "--benchmark_display_aggregates_only=true"
                ]
                , check=True 
            )
            self.cleanCSVOutput(resultPath)
        except subprocess.CalledProcessError as e:
            print(f'Failed to run benchmarks for {bench}')
        
    
    def runRange(self, bench, lowerBoundary, upperBoundary):
        for N in range(lowerBoundary, upperBoundary+1):
            pattern = rf"^BM_.*<[^>]*,[[:space:]]*{N}>($|/)"
            print("--benchmark_filter=", pattern)
            self.nbeapDim = N # setting this here to be used to name the output file
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