'''
What is the problem?
I want to automatically plot a graph of all data structure i am testing
What do i have?
I have the directory all the benchmark result is stored

Solution ?
- Check if the directory is empty?
- For each file in the directory
    - Each data structure has its name in the directory
    - So we can collect result for each benchmark for each file in an hashmap

    Assumptions
    - Each data structure only have one benchmark result file
'''
# First thing get the directory and check if it is empty


import re 
import os
from pathlib import Path 
import pandas as pd
import matplotlib.pyplot as plt


dir = "benchmark_results/"
main_map = {}
operations = ["Search", "Extract", "PushSortedAsc", "PushSortedDesc"]
DS = []
path = Path(dir)

if not any(path.iterdir()):
    print("not end")
    exit()

# draws the graph
def drawGraph():
    for op in operations:
        # what do i want to do?
        # 
        plt.figure(figsize=(10,6))
        plt.title(f"Plot of {op} operations")
        plt.xlabel("Input Size")
        plt.ylabel("Average time for operation in ns")
        plt.xscale("log", base=2)
        plt.ylim(bottom=0)

        for ds in DS:
            time = main_map[ds][op]["real_time"] 
            if op != "Search":
                time = time / main_map[ds][op]["Size"]
            plt.plot(main_map[ds][op]["Size"], time, marker='o', label=ds)
            

        plt.legend()
        plt.tight_layout()
        plt.show()
        plt.savefig(f"{op}_{ds}")

# takes the name of the DS and the path to the corresponding
# benchmark file
def processFile(name, path):
    if not path.is_file():
        print(f"{path} is not a file")
        return 
    
    df = pd.read_csv(path)
    df = df.dropna(axis=1, how="all")
    df["Size"] = df["name"].str.split("/").str[1].astype("float64")
    df["Operation"] = df["name"].str.extract(r"BM_([^/<]+)")
    df = df.drop("name", axis=1)
    groups = df.groupby("Operation")
    main_map[name] = main_map[name] = {op: df_op for op, df_op in groups}
    print("Finished extracting: ", path)



for p in path.iterdir():
    # i want to do something right here
    # get the name of the file
    pattern = r".*/bench_(.*).csv"
    name = re.search(pattern, str(p)).group(1)
    DS.append(name)
    # now that i know the name, i can analyse each file in detail
    processFile(name, p)




drawGraph()
    
print("end")