#!/usr/bin/env python3

# Caches intermediate benchmark results to track performance difference 
# across different implementations
# command - ./cache.py 3-nbeap (optional message)

import sys
import os
from pathlib import Path 
import pandas as pd 
import re 

cache_dir = "bench_results_cache"
benchmark_dir = "benchmark_results"


# get the csv file from benchmark_results dir
# get the cache file from the benchmark_result cache otherwise create a new one
def process(filename, message):
    bench = re.search(r".*/bench_(.*)\.csv$", str(filename)).group(1)
    dest_path = Path(cache_dir) / f"{bench}.csv"
    df_result = pd.read_csv(filename)
    df_result["size"] = df_result["name"].str.split("/").str[1].astype("float64")
    df_result["op"] = df_result["name"].str.extract(r"BM_([^/<]+)")
    df_out = df_result[["op", "size", "cpu_time"]]

    if not dest_path.exists():
        df_out.to_csv(dest_path, index=False)
    else:
        df_dest = pd.read_csv(dest_path)
        l = len(df_dest.columns) - 1
        df_dest = pd.concat(
            [df_dest, df_result["cpu_time"].rename(f"cpu_time_{l}{message}")], 
            axis=1
        )
        df_dest.to_csv(dest_path, index=False)

    


if __name__ == "__main__":  
    if len(sys.argv) <= 1:
        print("Error: Need to include file name and commit message")
        exit()
    
    bench_file = sys.argv[1]
    message = ""
    if len(sys.argv) > 2:
        message = sys.argv[2]
    
    path = Path(benchmark_dir)
    pattern = rf"^bench_(?:\d+-)?{re.escape(bench_file)}\.csv$"
    # filter out the benchmark result file from the dir
    files = [f for f in path.iterdir() if f.is_file() and re.match(pattern, f.name)]
    print(f"Processing {len(files)} files")

    for f in files:
        process(f, message)
    
    print("end")
    

